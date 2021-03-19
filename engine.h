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
 private:
  std::string StrPurchaseHead(int32_t server_categories_num);
  std::string StrPurchaseItem(info::HostTypeID host_type_id, int32_t server_num);
  std::string StrMigrationHead(int32_t migration_num);
  std::string StrMigrationItem(info::VMInstanceID vm_instance_id, info::HostInstanceID dst_host_instance_id, info::DeployPos deploy_pos);
  std::string StrAddItem(info::HostInstanceID target_server_id, info::DeployPos deploy_pos);

 public:
  std::vector<info::HostInfo> host_info_ary;
  std::map<std::string,info::HostTypeID> host_info_name2id_map;

  std::vector<info::VMInfo> vm_info_ary;            //保存虚拟机在种类
  std::map<std::string,info::VMTypeID> vm_info_name2id_map;  //虚拟机<名字,在数组的位置下标>

  std::vector<std::vector<info::Operate>> daily_ops_ary;

 public:
  std::map<int,info::VMInstanceID> vm_read2iid; //文本中 vm_instance_id 到 代码中的顺序 id
  std::vector<int> vm_iid2read;
};

#endif //HW005__ENGINE_H_
