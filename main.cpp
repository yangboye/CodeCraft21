#include <iostream>
#include "engine.h"

#define TEST

void local_debug_init(bool debug){
  if(debug) {
    freopen("/disks/disk0/yeyangbo/cpp_code/CodeCraft21/training-data/training-1.txt", "r", stdin);
  }
}


using namespace std;
int main() {
#ifdef TEST
  local_debug_init(true);
#else
  local_debug_init(false);
#endif
  Engine engine;
  engine.InitFromFILE(stdin);

  vector<std::vector<info::HostInfo>> host_deployed_ary(engine.host_info_ary.size());
  vector<info::VMDeployInfo> vm_deployed_ary(engine.vm_iid2read.size());  // 虚拟机部署的信息

  int cost = 0;
  for(int i=0;i<engine.daily_ops_ary.size();i++) {
    auto day_cost =  engine.RunOneDay000(i, host_deployed_ary, vm_deployed_ary);
    cost+=day_cost;
  }



  vector<string> res;

  engine.BuildOutput(res,host_deployed_ary, vm_deployed_ary);
  for(auto str:res){
    printf("%s\n",str.c_str());
  }

#ifdef TEST
  // 服务器能耗花费
  int power_cost = 0;
  int day = engine.daily_ops_ary.size();
  for(auto& hosts: host_deployed_ary) {
    for(auto& host : hosts) {
      power_cost += host.Cost(day);
    }
  }
  cout << "\n-------------- Cost -------------" << endl;
  cout << "Hardware cost: " << cost<< endl;
  cout << "Power cost: " << power_cost << endl;
  cout << "Total cost: " << (cost + power_cost) << endl;
#endif
  return 0;
}
