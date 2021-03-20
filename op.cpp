//
// Created by ydar on 2021/3/15.
//

#include "op.h"
info::Operate::Operate(const std::string &line, const std::map<std::string, info::VMTypeID> &vm_name2id_map) {
  char op_name[32];
  char vm_nmae[32];
  sscanf(line.c_str(),"%s",op_name);
  if(op_name[0]=='a'){
    this->op_type=OpType::ADD;
    sscanf(line.c_str(),"%s%s%d",op_name,vm_nmae,&this->vm_instance_id);
    this->vm_type_id = vm_name2id_map.find(std::string(vm_nmae))->second;

  }else{
    this->op_type=OpType::DEL;
    sscanf(line.c_str(),"%s%d",op_name,&this->vm_instance_id);
  }
}
