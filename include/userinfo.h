#ifndef __NATASHA_USERINFO_H__
#define __NATASHA_USERINFO_H__

#include <assert.h>
#include <functional>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"

namespace natasha {

struct UserInfo {
  ::natashapb::UserGameLogicInfo* pLogicUser;
  void* pCurConfig;
};

}  // namespace natasha

#endif  // __NATASHA_USERINFO_H__