// =============================================================================
// Created by yangb on 2021/3/12.
// =============================================================================

#ifndef CODECRAFT21__MY_TYPEINFO_H
#define CODECRAFT21__MY_TYPEINFO_H

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>

using num_t = int32_t;

struct HardwareNode {   // 硬件信息
  num_t cpu_core;       // cpu核数
  num_t memory_size;    // 内存大小

  HardwareNode() : cpu_core(0), memory_size(0) {}
  HardwareNode(num_t _cpu_core, num_t _memory_size) : cpu_core(_cpu_core), memory_size(_memory_size) {}

  inline bool IsValid() const {
    return ((cpu_core >= 0) && (memory_size >= 0));
  }
};

struct ServerNode {   // 服务器信息
//  std::string server_type;    // 服务器型号
  std::vector<HardwareNode> server_nodes;   // 服务器结点
  num_t                     hardware_cost;  // 硬件成本
  num_t                     power_cost;     // 能耗成本

  ServerNode() : hardware_cost(0), power_cost(0) {
    server_nodes.clear();
    server_nodes.push_back(HardwareNode());    // [0]: 节点A
    server_nodes.push_back(HardwareNode());    // [1]: 节点B
  }

  ServerNode(num_t _cpu_core,
             num_t _memory_size,
             num_t _hardware_cost,
             num_t _power_cost)
      : hardware_cost(_hardware_cost), power_cost(_power_cost) {
    server_nodes.clear();
    server_nodes.push_back(HardwareNode(_cpu_core / 2, _memory_size / 2));   // [0]: 节点A
    server_nodes.push_back(HardwareNode(_cpu_core / 2, _memory_size / 2));   // [1]: 节点B
  }

  inline bool IsValid(){
    return (server_nodes[0].IsValid() && server_nodes[1].IsValid());
  }
};

struct VmNode {     // 虚拟机信息
//  std::string vm_type;        // 虚拟机型号
  num_t    cpu_core;       // 虚拟机cpu核数
  num_t    memory_size;    // 虚拟机内存大小
  bool     dual_node;      // 是否双节点

  VmNode() : cpu_core(0), memory_size(0), dual_node(false) {}
  VmNode(num_t _cpu_core,
         num_t _memory_size, bool _dual_node) : cpu_core(_cpu_core), memory_size(_memory_size), dual_node(_dual_node) {}

  inline bool IsValid() const {
    return (cpu_core >= 0 && memory_size >= 0);
  }
};


// 选cpu单价最便宜的
bool cmp_cal(const std::pair<std::string, ServerNode>& s1, const std::pair<std::string, ServerNode>& s2) {
  // cpu单价, TODO: 未考虑每天的能耗费用
  float cpu_unit_price_1 = 1.0 * s1.second.hardware_cost / s1.second.server_nodes[0].cpu_core;
  float cpu_unit_price_2 = 1.0 * s2.second.hardware_cost / s2.second.server_nodes[0].cpu_core;
  if(fabs(cpu_unit_price_1 - cpu_unit_price_2) > 1e-6) {
    return cpu_unit_price_1 < cpu_unit_price_2;
  } else {
    // 内存单价
    float mem_unit_price_1 = 1.0 * s1.second.hardware_cost / s1.second.server_nodes[0].memory_size;
    float mem_unit_price_2 = 1.0 * s2.second.hardware_cost / s2.second.server_nodes[0].memory_size;
    return mem_unit_price_1 < mem_unit_price_2;
  }
}

// 选内存单价最便宜的
bool cmp_io(const std::pair<std::string, ServerNode>& s1, const std::pair<std::string, ServerNode>& s2) {
  // cpu单价
  float mem_unit_price_1 = 1.0 * s1.second.hardware_cost / s1.second.server_nodes[0].memory_size;
  float mem_unit_price_2 = 1.0 * s2.second.hardware_cost / s2.second.server_nodes[0].memory_size;
  if(fabs(mem_unit_price_1 - mem_unit_price_2) > 1e-6) {
    return mem_unit_price_1 < mem_unit_price_2;
  } else {
    // 内存单价
    float cpu_unit_price_1 = 1.0 * s1.second.hardware_cost / s1.second.server_nodes[0].cpu_core;
    float cpu_unit_price_2 = 1.0 * s2.second.hardware_cost / s2.second.server_nodes[0].cpu_core;
    return cpu_unit_price_1 < cpu_unit_price_2;
  }
}

// 平衡
bool cmp_balance(const std::pair<std::string, ServerNode>& s1, const std::pair<std::string, ServerNode>& s2) {
  // cpu单价
  float mem_unit_price_1 = 1.0 * s1.second.hardware_cost / s1.second.server_nodes[0].memory_size;
  float mem_unit_price_2 = 1.0 * s2.second.hardware_cost / s2.second.server_nodes[0].memory_size;
  // 内存单价
  float cpu_unit_price_1 = 1.0 * s1.second.hardware_cost / s1.second.server_nodes[0].cpu_core;
  float cpu_unit_price_2 = 1.0 * s2.second.hardware_cost / s2.second.server_nodes[0].cpu_core;
  return (cpu_unit_price_1+mem_unit_price_1) < (cpu_unit_price_2 + mem_unit_price_2);
}
#endif //CODECRAFT21__MY_TYPEINFO_H
