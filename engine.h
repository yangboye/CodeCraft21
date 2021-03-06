//
// Created by ydar on 2021/3/19.
//

#ifndef HW005__ENGINE_H_
#define HW005__ENGINE_H_

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <cstdio>

#include "op.h"
#include "tools.h"
#include "vm_info.h"
#include "typedef.h"
#include "host_info.h"

class Engine {
 public:
  Engine();
  void InitFromFILE(FILE* in);
  void BuildOutput(std::vector<std::string>& res,std::vector<std::vector<info::HostInfo>> &host_deployed_ary,std::vector<info::VMDeployInfo> &vm_deployed_ary);

  //默认的FF
  int32_t RunOneDay000(int32_t current_day,std::vector<std::vector<info::HostInfo>> &host_deployed_ary,std::vector<info::VMDeployInfo> &vm_deployed_ary);

  //BF min(w*dc+dm)
  int32_t RunOneDay001(int32_t current_day,
                       std::vector<std::vector<info::HostInfo>> &host_deployed_ary,
                       std::vector<info::VMDeployInfo> &vm_deployed_ary);

  //BF min(w*dc+dm)
  int32_t RunOneDay002(int32_t current_day,
                       std::vector<std::vector<info::HostInfo>> &host_deployed_ary,
                       std::vector<info::VMDeployInfo> &vm_deployed_ary,
                       std::vector<std::pair<info::HostTypeID,int32_t/*vec_pos*/>>& purchase_order_vec);

  //BF min(w*dc+dm)
  int32_t RunOneDay003(int32_t current_day,
                       std::vector<std::vector<info::HostInfo>> &host_deployed_ary,
                       std::vector<info::VMDeployInfo> &vm_deployed_ary,
                       std::vector<std::pair<info::HostTypeID,int32_t/*vec_pos*/>>& purchase_order_vec);

  inline int32_t GetMaxMigrationNum() const {
    return this->current_vm_num * 5 / 1000;
  }

  void Migrate(int32_t current_day,
               int32_t migration_num,
               std::vector<std::vector<info::HostInfo>> &host_deployed_ary,
               std::vector<info::VMDeployInfo> &vm_deployed_ary,
               std::vector<std::pair<info::HostTypeID,int32_t/*vec_pos*/>>& purchase_order_vec);

 private:
  // all this functions are implemented in `engine_base.cpp`
  // 购买服务器种类 e.g. (purchase, 2)
  std::string StrPurchaseHead(int32_t server_categories_num);
  // 购买的服务器种类-数量 e.g. (NV603, 1)
  std::string StrPurchaseItem(info::HostTypeID host_type_id, int32_t server_num);
  // 虚拟机迁移 数量 e.g. (migration, 2)
  std::string StrMigrationHead(int32_t migration_num);
  // 迁移 单节点(虚拟机 ID, 目的服务器 ID, 目的服务器节点) or 双节点(虚拟机 ID, 目的服务器 ID)
  std::string StrMigrationItem(info::VMInstanceID vm_instance_id, info::HostInstanceID dst_host_instance_id, info::DeployPos deploy_pos);
  // 虚拟机部署 单节点(服务器ID, 部署节点) or 双节点(服务器ID)
  std::string StrAddItem(info::HostInstanceID target_server_id, info::DeployPos deploy_pos);

 public:
  std::vector<info::HostInfo> host_info_ary;
  std::map<std::string,info::HostTypeID> host_info_name2id_map;

  std::vector<info::VMInfo> vm_info_ary;            //保存虚拟机在种类
  std::map<std::string,info::VMTypeID> vm_info_name2id_map;  //虚拟机<名字,在数组的位置下标>

  std::vector<std::vector<info::Operate>> daily_ops_ary;

 public:
  std::map<int,info::VMInstanceID> vm_read2iid; //文本中 vm_instance_id 到 代码中的顺序 id(GID产生)
  std::vector<int> vm_iid2read; // 代码中的顺序 ID（vector下标） -> 虚拟机ID

  int32_t current_vm_num;  // 当前虚拟机个数
};

#endif //HW005__ENGINE_H_
