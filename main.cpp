#include <iostream>
#include "engine.h"

void local_debug_init(bool debug){
  if(debug) {
    freopen("/disks/disk0/yeyangbo/cpp_code/CodeCraft21/training-data/training-1.txt", "r", stdin);
  }
}


using namespace std;
int main() {
  local_debug_init(true);
  Engine engine;
  engine.InitFromFILE(stdin);

  vector<std::vector<info::HostInfo>> host_deployed_ary(engine.host_info_ary.size());
  vector<info::VMDeployInfo> vm_deployed_ary(engine.vm_iid2read.size());

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

  cerr << cost<< endl;
  return 0;
}
