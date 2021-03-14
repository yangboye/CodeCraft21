#include <iostream>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <cassert>

#include <algorithm>

#include "my_typeinfo.h"

// 提交
//#define UPLOAD
// 测试
#define TEST

// ----------------------------------- 数据 -------------------------------------
#ifdef TEST
const std::string g_file_path = "/disks/disk0/yeyangbo/cpp_code/CodeCraft21/training-data/training-1.txt";  // 数据信息, 仅在Debug模式下用
#endif

// 服务器信息
std::unordered_map<std::string, ServerNode> g_server_info;
// 虚拟机信息
std::unordered_map<std::string, VmNode> g_vm_info;
// 请求信息. size=2: del请求, size=3: add请求
std::vector<std::vector<std::string>> g_request_info;

// 购买的服务器信息
int g_server_num = 0; // 当前已有的服务器数量
std::unordered_map<int, ServerNode> g_sys_server_resource;  // 服务器资源

// 当前开机了的服务器, size为当前服务器的总数
std::vector<int> g_server_running_vms;
// 记录虚拟机运行在哪个服务器上
std::unordered_map<std::string, std::vector<int>> g_vm_on_which_server;

// 购买服务器硬件的总成本
int64_t g_server_cost = 0;
// 运行服务器能耗的总成本
int64_t g_power_cost = 0;

// 最后的输出文本
std::vector<std::string> g_res;

// 计算密集型性价比最高的服务器类型
std::string g_cal_intensive_server_type;
// io密集型性价比最高的服务器类型
std::string g_io_intensive_server_type;
// 平衡型服务器类型
std::string g_balance_server_type;
void GetHighIntensiveServerType() {
  std::vector<std::pair<std::string, ServerNode>> servers(g_server_info.begin(), g_server_info.end());
  sort(servers.begin(), servers.end(), cmp_cal);
  g_cal_intensive_server_type = servers[0].first;
  sort(servers.begin(), servers.end(), cmp_io);
  g_io_intensive_server_type = servers[0].first;
  sort(servers.begin(), servers.end(), cmp_balance);
  g_balance_server_type = servers[0].first;
}

// 处理服务器类型数据
void ProcessServerDataFormat(const std::string& server_type,
                             const std::string& cpu_core,
                             const std::string& memory_size,
                             const std::string& hardware_cost,
                             const std::string& power_cost) {
  std::string temp_serv_type("");
  for (int i = 1; i < server_type.size() - 1; ++i) {
    temp_serv_type += server_type[i];
  }

  int temp_cpu_core = 0;
  for (int i = 0; i < cpu_core.size() - 1; ++i) {
    temp_cpu_core = 10 * temp_cpu_core + cpu_core[i] - '0';
  }

  int temp_memory_size = 0;
  for (int i = 0; i < memory_size.size() - 1; ++i) {
    temp_memory_size = 10 * temp_memory_size + memory_size[i] - '0';
  }

  int temp_hardware_cost = 0;
  for (int i = 0; i < hardware_cost.size() - 1; ++i) {
    temp_hardware_cost = 10 * temp_hardware_cost + hardware_cost[i] - '0';
  }

  int temp_power_cost = 0;
  for (int i = 0; i < power_cost.size() - 1; ++i) {
    temp_power_cost = 10 * temp_power_cost + power_cost[i] - '0';
  }

  g_server_info[temp_serv_type] = ServerNode(temp_cpu_core, temp_memory_size, temp_hardware_cost, temp_power_cost);
}


// 处理虚拟机类型数据
void ProcessVmDataFormat(const std::string& vm_type,
                         const std::string& vm_cpu_core,
                         const std::string& vm_memory_size,
                         const std::string& vm_dual_node) {
  std::string temp_vm_type("");
  for (int i = 1; i < vm_type.size() - 1; ++i) {
    temp_vm_type += vm_type[i];
  }

  int temp_vm_cpu_core = 0;
  for (int i = 0; i < vm_cpu_core.size() - 1; ++i) {
    temp_vm_cpu_core = 10 * temp_vm_cpu_core + vm_cpu_core[i] - '0';
  }

  int temp_vm_memory_size = 0;
  for (int i = 0; i < vm_memory_size.size() - 1; ++i) {
    temp_vm_memory_size = 10 * temp_vm_memory_size + vm_memory_size[i] - '0';
  }

  bool temp_vm_dual_node = false;
  if ('1' == vm_dual_node[0]) {
    temp_vm_dual_node = true;
  }

  g_vm_info[temp_vm_type] = VmNode(temp_vm_cpu_core, temp_vm_memory_size, temp_vm_dual_node);
}


// 处理用户 add 请求
void ProcessRequestDataFormat(const std::string& operation,
                              const std::string& req_vm_type,
                              const std::string& req_vm_id) {
  std::string temp_operation, temp_req_vm_type, temp_req_vm_id;
  temp_operation = operation.substr(1, operation.size() - 1);
  temp_req_vm_type = req_vm_type.substr(0, req_vm_type.size() - 1);
  temp_req_vm_id = req_vm_id.substr(0, req_vm_id.size() - 1);
  // (add, 虚拟机型号, 虚拟机ID)
  g_request_info.push_back(std::vector<std::string>{temp_operation, temp_req_vm_type, temp_req_vm_id});
}


// 处理用户 del 请求
void ProcessRequestDataFormat(const std::string& operation,
                              const std::string& req_vm_id) {
  std::string temp_operation, temp_req_vm_id;
  temp_operation = operation.substr(1, operation.size() - 1);
  temp_req_vm_id = req_vm_id.substr(0, req_vm_id.size() - 1);
  // (del, 虚拟机ID)
  g_request_info.push_back(std::vector<std::string>{temp_operation, temp_req_vm_id});
}


// 初始化服务器，即第0天的服务器如何购买(TODO: 优化)
void InitServer() {
  int n = 2500; // 要购买的总数
  g_server_running_vms.resize(n, 0);
  std::string init_buy = "(purchase, ";
  init_buy += std::to_string(2) + ")\n";  // 买2种服务器
  g_res.push_back(init_buy);  // (purchase, 2)

  // 下面开始购买服务器
  // 购买第一种服务器
  std::string server_type = "hostUY41I";  // cpu核数多, 计算密集型
  g_res.push_back("(" + server_type + ", " + std::to_string(n / 2) + ")\n");
  for (int i = 0; i < n / 2; ++i) {
    g_sys_server_resource[g_server_num++] = g_server_info[server_type];
    g_server_cost += g_server_info[server_type].hardware_cost;
  }
  // 购买第二种服务器
  server_type = "host78BMY";  // 内存多, IO密集型
  g_res.push_back("(" + server_type + ", " + std::to_string(n / 2) + ")\n");
  for (int i = 0; i < n / 2; ++i) {
    g_sys_server_resource[g_server_num++] = g_server_info[server_type];
    g_server_cost += g_server_info[server_type].hardware_cost;
  }
}


// TODO: 扩容(当前版本不用，因为InitServer中已经买了足够多的服务器了)
void ExpanseServer() {
  std::string expan = "(purchase, 0)\n";
  g_res.push_back(expan);
}


// TODO: 迁移(当前版本不用，因为InitServer中已经买了足够多的服务器了)
void Migrate() {
  std::string mig = "(migration, 0)\n";
  g_res.push_back(mig);
}


// 尝试在服务器上分配虚拟机资源
bool TryAllocateOnServer(ServerNode& server, const VmNode& vm, int server_id, const std::string& vm_id) {
  num_t need_cores = vm.cpu_core;
  num_t need_mem_size = vm.memory_size;
  if (vm.dual_node) {  // 如果虚拟机是双节点部署
    need_cores /= 2;
    need_mem_size /= 2;
    // 尝试在该服务器中分配
    if (server.server_nodes[0].cpu_core >= need_cores && server.server_nodes[1].cpu_core >= need_cores
        && server.server_nodes[0].memory_size >= need_mem_size && server.server_nodes[1].memory_size >= need_mem_size) {
      server.server_nodes[0].cpu_core -= need_cores;
      server.server_nodes[0].memory_size -= need_mem_size;
      server.server_nodes[1].cpu_core -= need_cores;
      server.server_nodes[1].memory_size -= need_mem_size;
      g_vm_on_which_server[vm_id] = std::vector<int>{server_id, need_cores, need_mem_size, 'A', 'B'};
      g_res.push_back("(" + std::to_string(server_id) + ")\n");
      return true;
    } else {
      return false;
    }
  } else if (server.server_nodes[0].cpu_core >= need_cores &&
             server.server_nodes[0].memory_size >= need_mem_size) { // 尝试在该服务器的A节点创建
    server.server_nodes[0].cpu_core -= need_cores;
    server.server_nodes[0].memory_size -= need_mem_size;
    g_vm_on_which_server[vm_id] = std::vector<int>{server_id, need_cores, need_mem_size, 'A'};
    g_res.push_back("(" + std::to_string(server_id) + ", A)\n");
    return true;
  } else if (server.server_nodes[1].cpu_core >= need_cores &&
             server.server_nodes[1].memory_size >= need_mem_size) { // 尝试在该服务器的A节点创建
    server.server_nodes[1].cpu_core -= need_cores;
    server.server_nodes[1].memory_size -= need_mem_size;
    g_vm_on_which_server[vm_id] = std::vector<int>{server_id, need_cores, need_mem_size, 'B'};
    g_res.push_back("(" + std::to_string(server_id) + ", B)\n");
    return true;
  }
  // 该服务器中放不下, 返回false
  return false;
}

// 创建虚拟机, req_info (add, 虚拟机型号, 虚拟机ID)
bool CreateVm(const std::vector<std::string>& req_info) {
  bool status = false;
  std::string req_vm_type = req_info[1];  // 请求的虚拟机型号
  std::string req_vm_id = req_info[2];  // 请求的虚拟机ID
  VmNode vm = g_vm_info[req_vm_type];

  for (int i = 0; i < g_server_num; ++i) {
    ServerNode& server = g_sys_server_resource[i];
    if (TryAllocateOnServer(server, vm, i, req_vm_id)) {
      g_server_running_vms[i]++;
      status = true;  // 分配成功
      break;
    }
    assert(server.IsValid());
  }

  return status;
}


// 删除虚拟机, req_info (del, 虚拟机ID)
void DeleteVm(const std::vector<std::string>& req_info) {
  std::string req_vm_id = req_info[1];
  std::vector<int> vm_on_server = g_vm_on_which_server[req_vm_id];
  int server_id = vm_on_server[0];

  ServerNode& server = g_sys_server_resource[server_id];
  if (5 == vm_on_server.size()) {  // 双节点部署 {server_id, cpu_core on per node, mem_size on per node, 'A', 'B'}
    server.server_nodes[0].cpu_core += vm_on_server[1];
    server.server_nodes[1].cpu_core += vm_on_server[1];
    server.server_nodes[0].memory_size += vm_on_server[2];
    server.server_nodes[1].memory_size += vm_on_server[2];
  } else {  // 单节点部署 {server_id, cpu_core, mem_size, node}
    if ('A' == vm_on_server[3]) {
      server.server_nodes[0].cpu_core += vm_on_server[1];
      server.server_nodes[0].memory_size += vm_on_server[2];
    } else {
      server.server_nodes[1].cpu_core += vm_on_server[1];
      server.server_nodes[1].memory_size += vm_on_server[2];
    }
  }
  g_server_running_vms[server_id]--;  // 运行在该服务器上的虚拟机减1
}


// 分配
void Allocate(int today) {
  // 根据每天的请求来调度服务器. 分配策略的步骤：
  // 1. 扩容
  // 2. 迁移(同一个服务器可以两个节点间迁移), 最多迁移 千分之五
  // 3. 处理请求

  // 1. 扩容
  if (today != 0) {  // 第0天的已经在InitServer中买了
    ExpanseServer();
  }

  // 2. 迁移
  Migrate();

  // 3. 处理请求
  for (auto& req: g_request_info) {
    bool is_add_opt = (req.size() == 3);
    if (is_add_opt) {  // add 虚拟机操作
      assert (CreateVm(req));
    } else {  // del 虚拟机操作
      DeleteVm(req);
    }
  }
}


void CalculatePowerCost() {
  for (int i = 0; i < g_server_num; ++i) {
    if (g_server_running_vms[i] != 0) {
      g_power_cost += g_sys_server_resource[i].power_cost;
    }
  }
}


int main() {
#ifdef TEST
  std::freopen(g_file_path.c_str(), "rb", stdin);
#endif
  int num_server_type;  // N: [1, 100], 可以采购的服务器类型数量
  scanf("%d", &num_server_type);
//  std::cin >> num_server_type;
  // 服务器信息
  std::string server_type, cpu_core, memory_size, hardware_cost, power_cost;
  for (int i = 0; i < num_server_type; ++i) {
    std::cin >> server_type >> cpu_core >> memory_size >> hardware_cost >> power_cost;
    ProcessServerDataFormat(server_type, cpu_core, memory_size, hardware_cost, power_cost);
  }

  GetHighIntensiveServerType(); // 获取不同性价比的服务器类型

  int num_vm_type;  // M: [1, 1000], 售卖的虚拟机类型数量
  scanf("%d", &num_vm_type);
  // 虚拟机信息
  std::string vm_type, vm_cpu_core, vm_memory_size, vm_dual_node;
  for (int i = 0; i < num_vm_type; ++i) {
    std::cin >> vm_type >> vm_cpu_core >> vm_memory_size >> vm_dual_node;
    ProcessVmDataFormat(vm_type, vm_cpu_core, vm_memory_size, vm_dual_node);
  }

  int num_days; // T: [1, 1000], 表示题目共会给出 T 天的用户请求序列数据
  scanf("%d", &num_days);

  // 初始化服务器，即第0天的服务器如何购买
  InitServer();

  // 每一天的用户请求序列
  int num_today_requests;   // R: 表示当天共有 R 条请求
  std::string operation;    // 操作: (add, 虚拟机型号, 虚拟机ID) or (del, 虚拟机ID)
  std::string req_vm_type;  // 请求的虚拟机型号
  std::string req_vm_id;    // 请求的虚拟机ID
  for (int today = 0; today < num_days; ++today) {
    scanf("%d", &num_today_requests);
    g_request_info.clear();

    for (int i = 0; i < num_today_requests; ++i) {
      std::cin >> operation;
      if ('a' == operation[1]) { // 如果是 add 操作
        std::cin >> req_vm_type >> req_vm_id;
        ProcessRequestDataFormat(operation, req_vm_type, req_vm_id);
      } else {
        std::cin >> req_vm_id;
        ProcessRequestDataFormat(operation, req_vm_id);
      }
    } // for

    // 根据每天的请求来调度服务器
    // 1. 扩容
    // 2. 迁移(同一个服务器可以两个节点间迁移), 最多迁移 千分之五
    // 3. 处理请求
    Allocate(today);

    // 计算当天的服务器消耗成本
    CalculatePowerCost();

  } // for

  fclose(stdin);

#ifdef UPLOAD
  for (auto& s:g_res) {
    std::cout << s;
  }
#endif

#ifdef TEST
  // 总成本
  int64_t total_cost = g_server_cost + g_power_cost;
  std::cout << "Server cost: " << g_server_cost << std::endl
            << "Power cost: "  << g_power_cost << std::endl
            << "Total cost: "  << total_cost << std::endl;
#endif
  return 0;
}
