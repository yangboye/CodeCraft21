//
// Created by ydar on 2021/3/15.
//

#ifndef HW001__TYPEDEF_H_
#define HW001__TYPEDEF_H_

#include<cstdint>

namespace info {

typedef int32_t HostTypeID ;     //服务器类型ID
typedef int32_t HostInstanceID ; //服务器实例ID

typedef int32_t VMTypeID ;     //虚拟机类型ID
typedef int32_t VMInstanceID ; //虚拟机实例ID

enum class DeployPos {
  A = 'A',
  B = 'B',
  ALL = 0,
};

enum class DeployType {
  Single,
  Double
};

enum class OpType{
  ADD=0,
  DEL=1
};

}

#endif //HW001__TYPEDEF_H_
