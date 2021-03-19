//
// Created by ydar on 2021/3/15.
//

#ifndef HW001__OP_H_
#define HW001__OP_H_

#include "typedef.h"
#include <string>
#include <map>
namespace info {
struct Operate {
  info::OpType op_type; // add or del
  info::VMTypeID vm_type_id=-1;
  info::VMInstanceID  vm_instance_id=-1;
 public:
  Operate()=default;
  explicit Operate(const std::string&line,const std::map<std::string,info::VMTypeID>& vm_name2id_map);
};
}
#endif //HW001__OP_H_
