//
// Created by ydar on 2021/3/15.
//

#include "host_info.h"
#include "vm_info.h"
#include <cstdio>
#include <assert.h>

info::HostInfo::HostInfo(const std::string& line) {
  int _double_core;
  int _double_memory;
  int _price;
  int _daily_cost;
  char _name[32];

  sscanf(line.c_str(), "%s%d%d%d%d", _name, &_double_core, &_double_memory, &_price, &_daily_cost);

  this->a_core = _double_core / 2;
  this->a_memory = _double_memory / 2;

  this->b_core = _double_core / 2;
  this->b_memory = _double_memory / 2;

  this->name = _name;
  this->price = _price;
  this->daily_cost = _daily_cost;
}

void info::HostInfo::AddVM(info::VMInstanceID vm_instance_id, const info::VMInfo& vm_info, info::DeployPos pos,
                           int current_day) {
//  this->deployed_vm_set.insert(vm_instance_id);
  deployed_num++;
  if (pos == info::DeployPos::A) {
    this->a_core -= vm_info.core;
    this->a_memory -= vm_info.memory;
    this->node_a_deployed_vm_set.insert(vm_instance_id);
  } else if (pos == info::DeployPos::B) {
    this->b_core -= vm_info.core;
    this->b_memory -= vm_info.memory;
    this->node_b_deployed_vm_set.insert(vm_instance_id);
  } else if (pos == info::DeployPos::ALL) {
    this->a_core -= vm_info.core;
    this->a_memory -= vm_info.memory;
    this->b_core -= vm_info.core;
    this->b_memory -= vm_info.memory;
    this->dual_node_deployed_vm_set.insert(vm_instance_id);
  }

  if (deployed_num == 1) {
    service_begin = current_day;
  }

  assert(this->a_core >= 0);
  assert(this->b_core >= 0);
  assert(this->a_memory >= 0);
  assert(this->b_memory >= 0);
}

void info::HostInfo::DelVM(info::VMInstanceID vm_instance_id, const info::VMInfo& vm_info, info::DeployPos pos,
                           int current_day) {
  deployed_num--;
  if (pos == info::DeployPos::A) {
    this->a_core += vm_info.core;
    this->a_memory += vm_info.memory;
    this->node_a_deployed_vm_set.erase(vm_instance_id);
  } else if (pos == info::DeployPos::B) {
    this->b_core += vm_info.core;
    this->b_memory += vm_info.memory;
    this->node_b_deployed_vm_set.erase(vm_instance_id);
  } else if (pos == info::DeployPos::ALL) {
    this->a_core += vm_info.core;
    this->a_memory += vm_info.memory;
    this->b_core += vm_info.core;
    this->b_memory += vm_info.memory;
    this->dual_node_deployed_vm_set.erase(vm_instance_id);
  }

  if (deployed_num == 0) {
    service_day += (current_day - service_begin);
  }
}


bool info::HostInfo::Alloc(const info::VMInfo& vm_info, info::DeployPos& output_pos) const {
  bool is_can_alloc = false;
  if (vm_info.deploy_type == info::DeployType::Single) {
    if (this->a_memory >= vm_info.memory && this->a_core >= vm_info.core) {
      output_pos = info::DeployPos::A;
      is_can_alloc = true;
    } else if (this->b_memory >= vm_info.memory && this->b_core >= vm_info.core) {
      output_pos = info::DeployPos::B;
      is_can_alloc = true;
    }
  } else if (this->a_memory >= vm_info.memory && this->a_core >= vm_info.core && this->b_memory >= vm_info.memory &&
             this->b_core >= vm_info.core) {
    output_pos = info::DeployPos::ALL;
    is_can_alloc = true;
  }
  return is_can_alloc;
}

int32_t info::HostInfo::Cost(int total_days) const {
  int cost = 0;
  if (deployed_num == 0) {
    cost = service_day * this->daily_cost;
  } else {
    cost = (service_day + (total_days - service_begin)) * this->daily_cost;
  }
  return cost;
}

int32_t info::HostInfo::Score(const info::VMInfo& vm_info, info::DeployPos pos) const {
  int c = 100000;
  int m = 100000;
  if (pos == info::DeployPos::A) {
    c = a_core - vm_info.core;
    m = a_memory - vm_info.memory;
  } else if (pos == info::DeployPos::B) {
    c = b_core - vm_info.core;
    m = b_memory - vm_info.memory;
  } else {
    c = a_core - vm_info.core + b_core - vm_info.core;
    m = a_memory - vm_info.memory + b_memory - vm_info.memory;
  }
  return 2 * c + m;
}

bool info::HostInfo::TestMove(const info::VMInfo& vm_info, info::DeployPos from_node, info::DeployPos to_node) const {
  int before_score = this->BalanceScore();
  int32_t cpu_diff = 0;
  bool resource_enough = false;  // 判断迁移过去的cpu和内存是否足够
  if (info::DeployPos::A == from_node && info::DeployPos::B == to_node) {  // 从节点A迁移到节点B
    // (a_core - vm_core) - (b_core + vm_core)
    cpu_diff = this->a_core - this->b_core - 2 * vm_info.core;
    resource_enough = ((this->b_memory >= vm_info.memory) && (this->b_core >= vm_info.core));
  } else if(info::DeployPos::B == from_node && info::DeployPos::A == to_node) {  // 从节点B迁移到节点A
    // (a_core + vm_core) - (b_core - vm_core)
    cpu_diff = this->a_core - this->b_core + 2 * vm_info.core;
    resource_enough = ((this->a_memory >= vm_info.memory) && (this->a_core >= vm_info.core));
  }
  return resource_enough && (abs(cpu_diff) < abs(before_score)); // 迁移后是否更平衡了
}


