//
// Created by ydar on 2021/3/15.
//

#ifndef HW001__VM_INFO_H_
#define HW001__VM_INFO_H_

#include "typedef.h"
#include <string>
namespace info{

struct VMDeployInfo{
  int32_t host_type_id=-1;
  int32_t host_vec_pos =-1;
  info::DeployPos deploy_pos;

  info::VMTypeID vm_type_id=-1;
  info::HostInstanceID host_instance_id=-1;
};


struct VMInfo {
  int32_t core=0;
  int32_t memory=0;
  std::string name;
  info::DeployType deploy_type; // single or dual node

  VMTypeID type_id=-1;
  VMInstanceID instance_id=-1;
 public:
  VMInfo()=default;
  explicit VMInfo(const std::string& line);
};
}


#endif //HW001__VM_INFO_H_
