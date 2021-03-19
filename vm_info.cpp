//
// Created by ydar on 2021/3/15.
//

#include "vm_info.h"
#include <cstdio>

info::VMInfo::VMInfo(const std::string &line) {
  // ：(型号, CPU 核数, 内存大小, 是否双节点部署)。
  int _core;
  int _memory;
  char _name[32];
  int _type;

  sscanf(line.c_str(),"%s%d%d%d",_name,&_core,&_memory,&_type);

  this->core=_core;
  this->memory=_memory;
  this->name=_name;
  this->deploy_type = static_cast<info::DeployType>(_type);

  if(this->deploy_type==info::DeployType::Double){
    this->core/=2;
    this->memory/=2;
  }
}
