//
// Created by ydar on 2021/3/19.
//
#include "engine.h"


std::string Engine::StrPurchaseHead(int32_t server_categories_num) {
  char buffer[128];
  snprintf(buffer,128,"(purchase, %d)",server_categories_num);
  return std::string(buffer);
}

std::string Engine::StrPurchaseItem(info::HostTypeID host_type_id, int32_t server_num) {
  char buffer[128];
  snprintf(buffer,128,"(%s, %d)",this->host_info_ary[host_type_id].name.c_str(),server_num);
  return std::string(buffer);
}

std::string Engine::StrMigrationHead(int32_t migration_num) {
  char buffer[128];
  snprintf(buffer,128,"(migration, %d)",migration_num);
  return std::string(buffer);
}

std::string Engine::StrMigrationItem(info::VMInstanceID vm_instance_id,
                                     info::HostInstanceID dst_host_instance_id,
                                     info::DeployPos deploy_pos) {
  char buffer[128];
  if(deploy_pos==info::DeployPos::ALL) {
    snprintf(buffer, 128, "(%d, %d)", this->vm_iid2read[vm_instance_id], dst_host_instance_id);
  }else{
    snprintf(buffer, 128, "(%d, %d, %c)", this->vm_iid2read[vm_instance_id], dst_host_instance_id, char(deploy_pos));
  }
  return std::string(buffer);
}

std::string Engine::StrAddItem(info::HostInstanceID target_server_id, info::DeployPos deploy_pos) {
  char buffer[128];
  if(deploy_pos==info::DeployPos::ALL) {
    snprintf(buffer,128,"(%d)",target_server_id);
  }else{
    snprintf(buffer, 128, "(%d, %c)", target_server_id, char(deploy_pos));
  }
  return std::string(buffer);
}

