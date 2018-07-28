#pragma once
#include "GMockPlus.h"
#include <vector>
#include <Windows.h>

namespace testing {

class HookMgr {
 public:
  static HookMgr* GetInstance();
  ~HookMgr() {}

  void AddFuncHook(GMockPlus* mockInfo);
  void AddFuncUnhook(GMockPlus* mockInfo);

  bool HookAll();
  bool UnhookAll();

 private:
  HookMgr() {}

  bool UpdateAllThread();
  void CloseAllHandle();

 private:
  typedef std::vector<GMockPlus*> MockInfoVec;
  MockInfoVec m_hookInfos;
  MockInfoVec m_unhookInfos;
  std::vector<HANDLE> m_threads;
};

} // namespace testing {
