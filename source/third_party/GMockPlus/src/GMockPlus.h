#pragma once
#include <windows.h>
#include <crtdbg.h>
#include <set>

// WARNING:被Mock函数的起始位置不能下断点，否则会导致Hook失败！
#define GMOCKPLUS_CDECL(srcAddr, gmockObj, gmockMethodAddr, paramCount) \
  testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(srcAddr, &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::CDECL_CALL, paramCount)

#define GMOCKPLUS_STDCALL(srcAddr, gmockObj, gmockMethodAddr, paramCount)   \
  testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(srcAddr, &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::STD_CALL, paramCount)

#define GMOCKPLUS_THISCALL(srcMethodAddr, gmockObj, gmockMethodAddr, paramCount)    \
  testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(testing::internal::GetMethodAddr(srcMethodAddr), &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::THIS_CALL, paramCount)

#define GMOCKPLUS_STATICMETHODCALL(srcMethodAddr, gmockObj, gmockMethodAddr, paramCount)    \
  testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(testing::internal::GetMethodAddr(srcMethodAddr), &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::STATIC_METHOD_CALL, paramCount)

namespace testing {

#define GMOCKPLUS_CONCAT_TOKEN_(foo, bar)   GMOCKPLUS_CONCAT_TOKEN_IMPL_(foo, bar)
#define GMOCKPLUS_CONCAT_TOKEN_IMPL_(foo, bar)  foo ## bar

// WARNING:这里不支持带可变参数的函数。
// 另外，可变参数的成员函数的栈平衡工作不再由成员函数本身来做了（这很合理，因为调用者才会明确的知道有多少个参数）
// 另外，调用可变参数的成员函数时，也没有再用ecx寄存器来传递this指针了。
enum CallType {
  INVALID_CALL = -1,
  STD_CALL = 0,
  CDECL_CALL = 1,     // __cdecl 调用方式的栈平衡工作是交由调用者来做的，此时不需要关心参数个数
  THIS_CALL = STD_CALL,
  STATIC_METHOD_CALL = CDECL_CALL,
  STATIC_STDCALL_METHOD_CALL = STD_CALL
};

class HookMgr;
class MockMgr;

class GMockPlus {
  friend class HookMgr;
  friend class MockMgr;
 public:
  GMockPlus(void* srcAddr, void* objPtr, void* dstMethodAddr, bool hookImmediately = true, CallType callType = CDECL_CALL, size_t paramCnt = 0, bool passParams = true);
  GMockPlus(void* srcAddr, void* dstAddr, bool hookImmediately = true);   // dstAddr 自己做栈平衡

  ~GMockPlus();

  void PreHook();
  void Hook();
  void PreUnhook();
  void Unhook();
  void* GetSrcAddr() {
    return m_srcAddr;
  }
  void* GetNewSrcAddr() {
    return m_newSrcAddr;
  }

  void* GetObjPtr() {
    return m_objPtr;
  }
  void* GetDstMethodAddr() {
    return m_dstMethodAddr;
  }
  size_t GetParamCount() {
    return m_paramCnt;
  }
  bool IsCDeclCall() {
    return m_callType == CDECL_CALL;
  }
  bool ShouldPassParams() {
    return m_passParams;
  }

 private:
  void* m_srcAddr;
  void* m_dstAddr;
  void* m_newSrcAddr;
  void* m_objPtr;
  void* m_dstMethodAddr;
  CallType m_callType;
  size_t m_paramCnt;
  bool m_hooked;
  bool m_registed;
  bool m_passParams;
};

#ifdef _DEBUG
#define ASSERT(expression)  \
  if (! (expression)) \
  {   \
    if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expression)) \
    {   \
      DebugBreak();   \
    }   \
  }
#else
#define ASSERT(expression)
#endif  // ~_DEBUG

namespace internal {
void* QueryNewSrcAddrImpl_(void* srcAddr);  // 找不到时直接返回srcAddr

template <typename T>
inline void* GetMethodAddr(T func) {
  return *(void**)&func;
}

}
void HookAll();
void UnhookAll();

// 鉴于 dbghelp 的限制，Symbol相关的函数“不”是线程安全的
bool SetSymSearchPath(const wchar_t* utf16Path);

/*
    @param
        moduleName 不带路径的模块名（如：xxx.dll, xxx.exe）
        symbolName 符号名（如：namespace_name::class_name::method_name）
    @return 符号地址
*/
// 对于非重载函数，参数symbolName直接传函数名即可，如：testFunc, MyClass::testMethod
// 如果是重载函数，则参数symbolName需要为函数名和参数类型信息，如：
// testFunc(int,int)
// MyClass::testMethod(int,int)
// testFunc<struct MyStruct>(char *,class MyClass &)
// std::_Construct<struct testing::internal::TraceInfo,struct testing::internal::TraceInfo>(struct testing::internal::TraceInfo *,struct testing::internal::TraceInfo const &)
// WARNING:需要严格注意symbolName的空格，不能多，也不能少
void* GetSymbolAddr(const char* moduleName, const char* symbolName);

// typedef std::map<std::string, void*> OverloadFuncName2FuncAddrMap; // 这个Map里的second字段目前看来没什么用，所以改成用Set来代替吧
typedef std::set<std::string> OverloadFuncNameSet;

// 如果不确定GetSymbolAddr函数的symbolName应该怎么写，可以调用下面这个函数来查询有哪些类似的函数
void QueryOverloadFuncNameSet(const char* moduleName, const char* symbolName, OverloadFuncNameSet& funcSet);

template<class FuncT>
FuncT QueryNewSrcAddr(FuncT srcAddr) {
  FuncT result = srcAddr;
  internal::QueryNewSrcAddrImpl_(internal::GetMethodAddr(srcAddr));
  __asm mov result, eax       // 编译器不让我们直接把 void* 转成类成员函数的函数指针，用汇编来解决！

  return result;
}

} // namespace testing {
