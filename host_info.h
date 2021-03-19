//
// Created by ydar on 2021/3/15.
//

#ifndef HW001__HOST_INFO_H_
#define HW001__HOST_INFO_H_

#include "typedef.h"
#include <string>
#include <set>
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
    
    int32_t deployed_num=0; // 运行在该服务器上的虚拟机数量
    int32_t service_day=0;  // 运行了多少天
    int32_t service_begin=-1; // 哪天开机的
   public:
    HostInfo()=default;
    explicit HostInfo(const std::string& line);

    void AddVM(info::VMInstanceID vm_instance_id,const info::VMInfo& vm_info,info::DeployPos pos,int current_day);
    void DelVM(info::VMInstanceID vm_instance_id,const info::VMInfo& vm_info,info::DeployPos pos,int current_day);
    // 判断当前服务器是否可以放下 虚拟机vm_info, 如果可以 返回true；output_pos: 该虚拟机放在服务器的位置{节点A, 节点B, 双节点}
    bool Alloc(const info::VMInfo& vm_info, info::DeployPos& output_pos) const;


    int32_t  Score(const info::VMInfo& vm_info,info::DeployPos pos) const;
    int32_t Cost(int total_days) const; // 计算到total_days为止, 该台服务器的能耗花费
  };

}



#endif //HW001__HOST_INFO_H_