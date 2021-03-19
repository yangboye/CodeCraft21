//
// Created by ydar on 2021/3/12.
//

#ifndef HW000__TOOLS_H_
#define HW000__TOOLS_H_

#include <string>
#include <cstdio>



namespace Tools {
class GID {
  int next_id;
 public:
  GID(){next_id=0;};
  inline int GetNextID(){
    return next_id++;
  }
  inline void Reset(){
    next_id=0;
  }
};
}



namespace Tools{
inline void ReplaceRubbish(char* line){
  char *p = line;
  while (*p!='\0'){
    if(*p=='('||*p==')'||*p==','){
      *p=' ';
    }
    p++;
  }
}

inline int32_t GetInt(const std::string& line){
  int32_t res=-1;
  sscanf(line.c_str(),"%d",&res);
  return res;
}

inline std::string GetOneLine(FILE* in){
  char str[256];
  char _;
  fscanf(in,"%[^\n]%*c",str);
  ReplaceRubbish(str);
  return str;
}

}




#endif //HW000__TOOLS_H_
