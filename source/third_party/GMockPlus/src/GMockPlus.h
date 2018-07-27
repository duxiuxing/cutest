#pragma once
#include <windows.h>
#include <crtdbg.h>
#include <set>

#include "gtest/gtest.h"

// WARNING:��Mock��������ʼλ�ò����¶ϵ㣬����ᵼ��Hookʧ�ܣ�
#define GMOCKPLUS_CDECL(srcAddr, gmockObj, gmockMethodAddr, paramCount)	\
	testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(srcAddr, &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::CDECL_CALL, paramCount)

#define GMOCKPLUS_STDCALL(srcAddr, gmockObj, gmockMethodAddr, paramCount)	\
	testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(srcAddr, &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::STD_CALL, paramCount)

#define GMOCKPLUS_THISCALL(srcMethodAddr, gmockObj, gmockMethodAddr, paramCount)	\
	testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(testing::internal::GetMethodAddr(srcMethodAddr), &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::THIS_CALL, paramCount)

#define GMOCKPLUS_STATICMETHODCALL(srcMethodAddr, gmockObj, gmockMethodAddr, paramCount)	\
	testing::GMockPlus GMOCKPLUS_CONCAT_TOKEN_(gmockplusObj_, __LINE__)(testing::internal::GetMethodAddr(srcMethodAddr), &gmockObj, testing::internal::GetMethodAddr(gmockMethodAddr), true, testing::STATIC_METHOD_CALL, paramCount)

//tolua_begin
namespace testing {
//tolua_end

#define GMOCKPLUS_CONCAT_TOKEN_(foo, bar)	GMOCKPLUS_CONCAT_TOKEN_IMPL_(foo, bar)
#define GMOCKPLUS_CONCAT_TOKEN_IMPL_(foo, bar)	foo ## bar

// WARNING:���ﲻ֧�ִ��ɱ�����ĺ�����
// ���⣬�ɱ�����ĳ�Ա������ջƽ�⹤�������ɳ�Ա�������������ˣ���ܺ�����Ϊ�����߲Ż���ȷ��֪���ж��ٸ�������
// ���⣬���ÿɱ�����ĳ�Ա����ʱ��Ҳû������ecx�Ĵ���������thisָ���ˡ�
enum CallType {
	INVALID_CALL = -1,
	STD_CALL = 0,
	CDECL_CALL = 1,		// __cdecl ���÷�ʽ��ջƽ�⹤���ǽ��ɵ����������ģ���ʱ����Ҫ���Ĳ�������
	THIS_CALL = STD_CALL,
	STATIC_METHOD_CALL = CDECL_CALL,
	STATIC_STDCALL_METHOD_CALL = STD_CALL
};

class HookMgr;
class MockMgr;

class GTEST_API_ GMockPlus {
	friend class HookMgr;
	friend class MockMgr;
public:
	GMockPlus(void* srcAddr, void* objPtr, void* dstMethodAddr, bool hookImmediately = true, CallType callType = CDECL_CALL, size_t paramCnt = 0, bool passParams = true);
	GMockPlus(void* srcAddr, void* dstAddr, bool hookImmediately = true);	// dstAddr �Լ���ջƽ��

	~GMockPlus();

	void PreHook();
	void Hook();
	void PreUnhook();
	void Unhook();
	void* GetSrcAddr() { return m_srcAddr; }
	void* GetNewSrcAddr() { return m_newSrcAddr; }

	void* GetObjPtr() { return m_objPtr; }
	void* GetDstMethodAddr() { return m_dstMethodAddr; }
	size_t GetParamCount() { return m_paramCnt; }
	bool IsCDeclCall() { return m_callType == CDECL_CALL; }
	bool ShouldPassParams() { return m_passParams; }

private:
	void*		m_srcAddr;
	void*		m_dstAddr;
	void*		m_newSrcAddr;
	void*		m_objPtr;
	void*		m_dstMethodAddr;
	CallType	m_callType;
	size_t		m_paramCnt;
	bool		m_hooked;
	bool		m_registed;
	bool		m_passParams;
};

#ifdef _DEBUG
#define ASSERT(expression)	\
	if (! (expression))	\
{	\
	if (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expression))	\
{	\
	DebugBreak();	\
}	\
}
#else
#define ASSERT(expression)
#endif	// ~_DEBUG

namespace internal {
GTEST_API_ void* QueryNewSrcAddrImpl_(void* srcAddr);	// �Ҳ���ʱֱ�ӷ���srcAddr

template <typename T>
inline void* GetMethodAddr(T func) {
	return *(void**)&func;
}

}
void HookAll();
void UnhookAll();

// ���� dbghelp �����ƣ�Symbol��صĺ������������̰߳�ȫ��
GTEST_API_ bool SetSymSearchPath(const wchar_t* utf16Path);

/*
	@param
		moduleName ����·����ģ�������磺xxx.dll, xxx.exe��
		symbolName ���������磺namespace_name::class_name::method_name��
	@return	���ŵ�ַ
*/
// ���ڷ����غ���������symbolNameֱ�Ӵ����������ɣ��磺testFunc, MyClass::testMethod
// ��������غ����������symbolName��ҪΪ�������Ͳ���������Ϣ���磺
// testFunc(int,int)
// MyClass::testMethod(int,int)
// testFunc<struct MyStruct>(char *,class MyClass &)
// std::_Construct<struct testing::internal::TraceInfo,struct testing::internal::TraceInfo>(struct testing::internal::TraceInfo *,struct testing::internal::TraceInfo const &)
// WARNING:��Ҫ�ϸ�ע��symbolName�Ŀո񣬲��ܶ࣬Ҳ������
GTEST_API_ void* GetSymbolAddr(const char* moduleName, const char* symbolName);

// typedef std::map<std::string, void*>	OverloadFuncName2FuncAddrMap; // ���Map���second�ֶ�Ŀǰ����ûʲô�ã����Ըĳ���Set�������
typedef std::set<std::string> OverloadFuncNameSet;

// �����ȷ��GetSymbolAddr������symbolNameӦ����ôд�����Ե������������������ѯ����Щ���Ƶĺ���
GTEST_API_ void QueryOverloadFuncNameSet(const char* moduleName, const char* symbolName, OverloadFuncNameSet& funcSet);

template<class FuncT>
FuncT QueryNewSrcAddr(FuncT srcAddr) {
	FuncT result = srcAddr;	
	internal::QueryNewSrcAddrImpl_(internal::GetMethodAddr(srcAddr));
	__asm mov result, eax		// ��������������ֱ�Ӱ� void* ת�����Ա�����ĺ���ָ�룬�û���������

	return result;
}
//tolua_begin
}	// ~testing
//tolua_end