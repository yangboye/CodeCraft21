//
// Created by ydar on 2021/3/15.
//

#ifndef HW001__HOST_INFO_H_
#define HW001__HOST_INFO_H_

#include "typedef.h"
#include <string>
#include <set>
#include <list>
namespace info{
  struct VMInfo;

  struct HostInfo{
    HostTypeID host_type_id=-1;
    HostInstanceID host_instance_id=-1;


    int32_t price=0;
    int32_t daily_cost=0;

    std::string name;

    int32_t a_core=0;   //核心数
    int32_t a_memory=0; //内存数

    int32_t b_core=0;   //核心数
    int32_t b_memory=0; //内存数
    
    int32_t deployed_num=0;
    int32_t service_day=0;
    int32_t service_begin=-1;


    std::set<info::VMInstanceID> node_a_deployed_vm_set; // 当前服务器节点B中放的虚拟机
    std::set<info::VMInstanceID> node_b_deployed_vm_set; // 当前服务器节点A中放的虚拟机
    std::set<info::VMInstanceID> dual_node_deployed_vm_set; // 当前服务器中放的双节点虚拟机
   public:
    HostInfo()=default;
    explicit HostInfo(const std::string& line);

    void AddVM(info::VMInstanceID vm_instance_id,const info::VMInfo& vm_info,info::DeployPos pos,int current_day);
    void DelVM(info::VMInstanceID vm_instance_id,const info::VMInfo& vm_info,info::DeployPos pos,int current_day);
    bool Alloc(const info::VMInfo& vm_info,info::DeployPos& output_pos) const;


    int32_t  Score(const info::VMInfo& vm_info,info::DeployPos pos) const;
    int32_t Cost(int total_days) const;

    inline int BalanceScore() const { // >0: A多B少  <0: B多A少
      int32_t cpu_diff = this->a_core - this->b_core;
//      int32_t mem_diff = this->a_memory - this->b_memory;
      return cpu_diff;
    }

    bool TestMove(const info::VMInfo& vm_info, info::DeployPos from_node, info::DeployPos to_node) const;
  };


}



#endif //HW001__HOST_INFO_H_
