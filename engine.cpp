//
// Created by ydar on 2021/3/19.
//

#include "engine.h"

#include <assert.h>
#include <algorithm>
#include <random>
#include <set>
#include <iostream>
using namespace std;

Engine::Engine() {
  host_info_ary.reserve(1'000);
  vm_info_ary.reserve(10'000);
  daily_ops_ary.reserve(2'000);
}

void Engine::InitFromFILE(FILE *in)
{
  {//host 输入
    this->host_info_ary.resize(Tools::GetInt(Tools::GetOneLine(in)));
    for (auto &node:host_info_ary) {
      node = info::HostInfo(Tools::GetOneLine(in));
    }
  }

  {//虚拟机输入
    this->vm_info_ary.resize(Tools::GetInt(Tools::GetOneLine(in)));
    for (auto &node:vm_info_ary) {
      node = info::VMInfo(Tools::GetOneLine(in));
    }
  }

  {//为 host 和 vm 生成 type_id
    sort(this->host_info_ary.begin(), this->host_info_ary.end(),
         [](const info::HostInfo &x, const info::HostInfo &y) {
           return (x.price < y.price) || (x.price == y.price && x.daily_cost < y.daily_cost);
         });

    this->host_info_name2id_map.clear();
    for (int i = 0; i < this->host_info_ary.size(); i++) {
      host_info_name2id_map[this->host_info_ary[i].name] = i;
      this->host_info_ary[i].host_type_id = i;
    }

    this->vm_info_name2id_map.clear();
    for (int i = 0; i < this->vm_info_ary.size(); i++) {
      vm_info_name2id_map[this->vm_info_ary[i].name] = i;
      this->vm_info_ary[i].type_id = i;
    }
  }


  { // 输入 daily op
    Tools::GID vm_iid_dist;
    daily_ops_ary.clear();
    this->daily_ops_ary.resize(Tools::GetInt(Tools::GetOneLine(in)));

    int total_day = this->daily_ops_ary.size();
    int current_day = 0;

    for (auto &day_ops:this->daily_ops_ary) {
      current_day++;

      day_ops.resize(Tools::GetInt(Tools::GetOneLine(in)));

      int op_pos = 0;
      for (auto &op:day_ops) {
        op_pos++;
        op = info::Operate(Tools::GetOneLine(in), this->vm_info_name2id_map);

        if(op.op_type==info::OpType::ADD){
          int new_id = vm_iid_dist.GetNextID();
          this->vm_read2iid[op.vm_instance_id] = new_id;

          assert(this->vm_iid2read.size()==new_id);
          this->vm_iid2read.push_back(op.vm_instance_id);
        }

        op.vm_instance_id = this->vm_read2iid[op.vm_instance_id];

      }
    }
  }
}

int32_t Engine::RunOneDay000(int32_t current_day,
                               vector<std::vector<info::HostInfo>> &host_deployed_ary,
                               vector<info::VMDeployInfo> &vm_deployed_ary) {

  const auto& ops = this->daily_ops_ary[current_day];

  int cost = 0;
  for(auto& op:ops){
    if(op.op_type==info::OpType::ADD){
      int deploy_host_type_id = -1;
      int deploy_vec_pos = -1;
      info::DeployPos deploy_pos ;

      const auto vm_info = this->vm_info_ary[op.vm_type_id];

      for(auto host_type_id=0;host_type_id<host_deployed_ary.size();host_type_id++){
        for(auto vec_pos=0;vec_pos<host_deployed_ary[host_type_id].size();vec_pos++){
          if(host_deployed_ary[host_type_id][vec_pos].Alloc(vm_info,deploy_pos)){
            deploy_host_type_id = host_type_id;
            deploy_vec_pos = vec_pos;
            break;
          }
        }
      }

      if(deploy_host_type_id==-1){
        // 寻找
        info::DeployPos find_deploy_pos ;
        for(auto host_type_id=0;host_type_id<host_info_ary.size();host_type_id++) {
          if(host_info_ary[host_type_id].Alloc(vm_info,find_deploy_pos)){
            deploy_host_type_id = host_type_id;
            break;
          }
        }

        // 添加 host instance
        deploy_pos = find_deploy_pos;
        deploy_vec_pos  = host_deployed_ary[deploy_host_type_id].size();
        host_deployed_ary[deploy_host_type_id].push_back(host_info_ary[deploy_host_type_id]);

        cost+=host_deployed_ary[deploy_host_type_id][deploy_vec_pos].price;
      }

      host_deployed_ary[deploy_host_type_id][deploy_vec_pos].AddVM(op.vm_instance_id,vm_info,deploy_pos,current_day);


      vm_deployed_ary[op.vm_instance_id].vm_type_id   = op.vm_type_id;
      vm_deployed_ary[op.vm_instance_id].host_type_id = deploy_host_type_id;
      vm_deployed_ary[op.vm_instance_id].host_vec_pos = deploy_vec_pos;
      vm_deployed_ary[op.vm_instance_id].deploy_pos   = deploy_pos;

    }else{
      auto vm_deploy_info = vm_deployed_ary[op.vm_instance_id];
      const auto vm_info = this->vm_info_ary[vm_deploy_info.vm_type_id];
      host_deployed_ary[vm_deploy_info.host_type_id][vm_deploy_info.host_vec_pos].DelVM(op.vm_instance_id,vm_info,vm_deploy_info.deploy_pos,current_day);

    }// if op.op_type
  }

  return cost;
}

void Engine::BuildOutput(vector<std::string> &res,
                         vector<std::vector<info::HostInfo>> &host_deployed_ary,
                         vector<info::VMDeployInfo> &vm_deployed_ary) {
  //给 Host 分配 ID
  vector<pair<info::HostTypeID,int>> total_buy;
  {
    Tools::GID host_iid_dist;
    for(int i=0;i<host_deployed_ary.size();i++){
      if(!host_deployed_ary[i].empty()){
        total_buy.emplace_back(i,host_deployed_ary[i].size());
        for(auto & j : host_deployed_ary[i]){
          j.host_instance_id = host_iid_dist.GetNextID();
        }
      }
    }
  }

  for(const auto&ops:this->daily_ops_ary){
    res.emplace_back(StrPurchaseHead(total_buy.size()));
    for(auto [id,num]:total_buy){
      res.emplace_back(StrPurchaseItem(id,num));
    }
    total_buy.clear();

    res.emplace_back(StrMigrationHead(0));

    for(auto op:ops){
      if(op.op_type==info::OpType::ADD){
        auto vm_deployed_info = vm_deployed_ary[op.vm_instance_id];
        res.emplace_back(StrAddItem(host_deployed_ary[vm_deployed_info.host_type_id][vm_deployed_info.host_vec_pos].host_instance_id,vm_deployed_info.deploy_pos));
      }
    }
  }
}

int32_t Engine::RunOneDay001(int32_t current_day,
                             vector<std::vector<info::HostInfo>> &host_deployed_ary,
                             vector<info::VMDeployInfo> &vm_deployed_ary) {
  const auto& ops = this->daily_ops_ary[current_day];

  int cost = 0;
  for(auto& op:ops){
    if(op.op_type==info::OpType::ADD){
      int deploy_host_type_id = -1;
      int deploy_vec_pos = -1;
      info::DeployPos deploy_pos ;

      const auto vm_info = this->vm_info_ary[op.vm_type_id];

      //寻找best
      info::DeployPos t_deploy_pos;
      int32_t score = INT32_MAX;

      for(auto host_type_id=0;host_type_id<host_deployed_ary.size();host_type_id++){
        for(auto vec_pos=0;vec_pos<host_deployed_ary[host_type_id].size();vec_pos++){
          if(host_deployed_ary[host_type_id][vec_pos].Alloc(vm_info,t_deploy_pos)){
            auto sss = host_deployed_ary[host_type_id][vec_pos].Score(vm_info,t_deploy_pos);
            if(sss<score){
              deploy_host_type_id = host_type_id;
              deploy_vec_pos = vec_pos;
              deploy_pos = t_deploy_pos;
              score = sss;
            }
          }
        }
      }

      if(deploy_host_type_id==-1){
        // 寻找
        info::DeployPos find_deploy_pos ;
        for(auto host_type_id=0;host_type_id<host_info_ary.size();host_type_id++) {
          if(host_info_ary[host_type_id].Alloc(vm_info,find_deploy_pos)){
            deploy_host_type_id = host_type_id;
            break;
          }
        }

        // 添加 host instance
        deploy_pos = find_deploy_pos;
        deploy_vec_pos  = host_deployed_ary[deploy_host_type_id].size();
        host_deployed_ary[deploy_host_type_id].push_back(host_info_ary[deploy_host_type_id]);

        cost+=host_deployed_ary[deploy_host_type_id][deploy_vec_pos].price;
      }

      host_deployed_ary[deploy_host_type_id][deploy_vec_pos].AddVM(op.vm_instance_id,vm_info,deploy_pos,current_day);


      vm_deployed_ary[op.vm_instance_id].vm_type_id   = op.vm_type_id;
      vm_deployed_ary[op.vm_instance_id].host_type_id = deploy_host_type_id;
      vm_deployed_ary[op.vm_instance_id].host_vec_pos = deploy_vec_pos;
      vm_deployed_ary[op.vm_instance_id].deploy_pos   = deploy_pos;

    }else{
      auto vm_deploy_info = vm_deployed_ary[op.vm_instance_id];
      const auto vm_info = this->vm_info_ary[vm_deploy_info.vm_type_id];
      host_deployed_ary[vm_deploy_info.host_type_id][vm_deploy_info.host_vec_pos].DelVM(op.vm_instance_id,vm_info,vm_deploy_info.deploy_pos,current_day);
    }// if op.op_type
  }

  return cost;
}

int32_t Engine::RunOneDay002(int32_t current_day,
                             vector<std::vector<info::HostInfo>> &host_deployed_ary,
                             vector<info::VMDeployInfo> &vm_deployed_ary,
                             vector<std::pair<info::HostTypeID, int32_t>> &purchase_order_vec) {
  const auto& ops = this->daily_ops_ary[current_day];

  int cost = 0;
  for(auto& op:ops){
    if(op.op_type==info::OpType::ADD){
      int deploy_host_type_id = -1;
      int deploy_vec_pos = -1;
      info::DeployPos deploy_pos ;

      const auto vm_info = this->vm_info_ary[op.vm_type_id];

      //寻找best
      info::DeployPos t_deploy_pos;
      int32_t score = INT32_MAX;

      for(auto [t_host_type_id,t_vec_pos]:purchase_order_vec){
        if(host_deployed_ary[t_host_type_id][t_vec_pos].Alloc(vm_info,t_deploy_pos)){
          auto sss = host_deployed_ary[t_host_type_id][t_vec_pos].Score(vm_info,t_deploy_pos);
          if(sss<score){
            deploy_host_type_id = t_host_type_id;
            deploy_vec_pos = t_vec_pos;
            deploy_pos = t_deploy_pos;
            score = sss;
          }
        }
      }

      if(deploy_host_type_id==-1){
        // 寻找
        info::DeployPos find_deploy_pos ;
        for(auto host_type_id=0;host_type_id<host_info_ary.size();host_type_id++) {
          if(host_info_ary[host_type_id].Alloc(vm_info,find_deploy_pos)){
            deploy_host_type_id = host_type_id;
            break;
          }
        }

        // 添加 host instance
        deploy_pos = find_deploy_pos;
        deploy_vec_pos  = host_deployed_ary[deploy_host_type_id].size();
        host_deployed_ary[deploy_host_type_id].push_back(host_info_ary[deploy_host_type_id]);

        purchase_order_vec.emplace_back(make_pair(deploy_host_type_id,deploy_vec_pos));

        cost+=host_deployed_ary[deploy_host_type_id][deploy_vec_pos].price;
      }

      host_deployed_ary[deploy_host_type_id][deploy_vec_pos].AddVM(op.vm_instance_id,vm_info,deploy_pos,current_day);


      vm_deployed_ary[op.vm_instance_id].vm_type_id   = op.vm_type_id;
      vm_deployed_ary[op.vm_instance_id].host_type_id = deploy_host_type_id;
      vm_deployed_ary[op.vm_instance_id].host_vec_pos = deploy_vec_pos;
      vm_deployed_ary[op.vm_instance_id].deploy_pos   = deploy_pos;

    }else{
      auto vm_deploy_info = vm_deployed_ary[op.vm_instance_id];
      const auto vm_info = this->vm_info_ary[vm_deploy_info.vm_type_id];
      host_deployed_ary[vm_deploy_info.host_type_id][vm_deploy_info.host_vec_pos].DelVM(op.vm_instance_id,vm_info,vm_deploy_info.deploy_pos,current_day);
    }// if op.op_type
  }

  return cost;
}

int32_t Engine::RunOneDay003(int32_t current_day,
                             vector<std::vector<info::HostInfo>> &host_deployed_ary,
                             vector<info::VMDeployInfo> &vm_deployed_ary,
                             vector<std::pair<info::HostTypeID, int32_t>> &purchase_order_vec) {
  const auto& ops = this->daily_ops_ary[current_day];

  set<std::pair<info::HostTypeID, int32_t>> new_purchase_order_set;
  map<info::HostTypeID,vector<int32_t>> new_purchase_order_map;

  int cost = 0;
  for(auto& op:ops){
    if(op.op_type==info::OpType::ADD){
      int deploy_host_type_id = -1;
      int deploy_vec_pos = -1;
      info::DeployPos deploy_pos ;

      const auto vm_info = this->vm_info_ary[op.vm_type_id];

      //寻找best
      info::DeployPos t_deploy_pos;
      int32_t score = INT32_MAX;

      for(auto [t_host_type_id,t_vec_pos]:purchase_order_vec){
        if(host_deployed_ary[t_host_type_id][t_vec_pos].Alloc(vm_info,t_deploy_pos)){
          auto sss = host_deployed_ary[t_host_type_id][t_vec_pos].Score(vm_info,t_deploy_pos);
          if(sss<score){
            deploy_host_type_id = t_host_type_id;
            deploy_vec_pos = t_vec_pos;
            deploy_pos = t_deploy_pos;
            score=sss;
          }
        }
      }

      if(deploy_host_type_id==-1) {
        //在新购买的中寻找
        for (auto[t_host_type_id, t_vec_pos_ary]:new_purchase_order_map) {
          for(auto t_vec_pos:t_vec_pos_ary) {
            if (host_deployed_ary[t_host_type_id][t_vec_pos].Alloc(vm_info, t_deploy_pos)) {
              deploy_host_type_id = t_host_type_id;
              deploy_vec_pos = t_vec_pos;
              deploy_pos = t_deploy_pos;
              break;
            }
          }
        }
      }

      if(deploy_host_type_id==-1) { //如果还是找到不到,那只能买新的
        // 寻找
        info::DeployPos find_deploy_pos ;
        for(auto host_type_id=0;host_type_id<host_info_ary.size();host_type_id++) {
          if(host_info_ary[host_type_id].Alloc(vm_info,find_deploy_pos)){
            deploy_host_type_id = host_type_id;
            break;
          }
        }

        // 添加 host instance
        deploy_pos = find_deploy_pos;
        deploy_vec_pos  = host_deployed_ary[deploy_host_type_id].size();
        host_deployed_ary[deploy_host_type_id].push_back(host_info_ary[deploy_host_type_id]); //购买的服务器 和 旧的放到一起

        new_purchase_order_set.insert(make_pair(deploy_host_type_id,deploy_vec_pos)); //购买的记录放到新的里面
        new_purchase_order_map[deploy_host_type_id].push_back(deploy_vec_pos);
        cost+=host_deployed_ary[deploy_host_type_id][deploy_vec_pos].price;
      }

      host_deployed_ary[deploy_host_type_id][deploy_vec_pos].AddVM(op.vm_instance_id,vm_info,deploy_pos,current_day);


      vm_deployed_ary[op.vm_instance_id].vm_type_id   = op.vm_type_id;
      vm_deployed_ary[op.vm_instance_id].host_type_id = deploy_host_type_id;
      vm_deployed_ary[op.vm_instance_id].host_vec_pos = deploy_vec_pos;
      vm_deployed_ary[op.vm_instance_id].deploy_pos   = deploy_pos;

    }else{
      auto vm_deploy_info = vm_deployed_ary[op.vm_instance_id];
      const auto vm_info = this->vm_info_ary[vm_deploy_info.vm_type_id];
      host_deployed_ary[vm_deploy_info.host_type_id][vm_deploy_info.host_vec_pos].DelVM(op.vm_instance_id,vm_info,vm_deploy_info.deploy_pos,current_day);
    }// if op.op_type
  }

  purchase_order_vec.insert(purchase_order_vec.end(),new_purchase_order_set.begin(),new_purchase_order_set.end());
  return cost;
}




