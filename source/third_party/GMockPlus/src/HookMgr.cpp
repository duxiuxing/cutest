#include "HookMgr.h"
#include <Windows.h>
#include "detours.h"
#include <tlhelp32.h>
#include "MockMgr.h"

namespace testing {

HookMgr* HookMgr::GetInstance() {
	static HookMgr* mgr = NULL;
	if (mgr == NULL) {
		mgr = new HookMgr;
	}

	return mgr;
}

void HookMgr::AddFuncHook(GMockPlus* info) {
	ASSERT(info != NULL);
	m_hookInfos.push_back(info);
}

void HookMgr::AddFuncUnhook(GMockPlus* info) {
	ASSERT(info != NULL);
	m_unhookInfos.push_back(info);
}

bool HookMgr::UpdateAllThread() {
	bool result = true;
	HANDLE        hThreadSnap = NULL;
	THREADENTRY32 te32        = {0};

	// Take a snapshot of all threads currently in the system. 
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
	if (hThreadSnap == INVALID_HANDLE_VALUE) 
		return false; 

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32); 

	// Walk the thread snapshot to find all threads of the process. 
	// If the thread belongs to the process, add its information 
	// to the display list.
	DWORD curProcID = GetCurrentProcessId();
	DWORD curThreadID = GetCurrentThreadId();
	if (Thread32First(hThreadSnap, &te32)) { 
		result = true;
		do { 
			if (te32.th32OwnerProcessID == curProcID) {
				if (te32.th32ThreadID == curThreadID)
					continue;
				DWORD right = THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME;
				HANDLE hThread = OpenThread(right, TRUE, te32.th32ThreadID);
				m_threads.push_back(hThread);
				if (DetourUpdateThread(hThread) != NO_ERROR) {
					result = false;
					break;
				}
			} 
		} while (Thread32Next(hThreadSnap, &te32));
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hThreadSnap);

	return result;
}

bool HookMgr::HookAll() {
	bool result = false;
	if (m_hookInfos.empty())
		return result;

	do {

		if (DetourTransactionBegin() != NO_ERROR)
			break;
		if (!UpdateAllThread())
			break;

		bool attachFailed = false;
		MockInfoVec::iterator it = m_hookInfos.begin();
		for (; !attachFailed && it != m_hookInfos.end(); ++it) {
			ASSERT(!(*it)->m_hooked);
			(*it)->m_newSrcAddr = (*it)->m_srcAddr;
			if (DetourAttach(&((*it)->m_newSrcAddr), (*it)->m_dstAddr) != NO_ERROR) {
				attachFailed = true;
				OutputDebugStringW(L"DetourAttach Failed! ��������Ϊ�ں�����ڴ������˶ϵ㣬�������ϵ��Ƴ������ԣ�");
			} else {
				(*it)->m_hooked = true;
				// ����commit֮ǰAdd�����commit֮���Add�Ļ����ܻᵼ�������̵߳��õ�MockFunction����ʱ�Ҳ�����Ӧ��srcFunc
				MockMgr::GetInstance()->AddMockFuncInfo(*it);
			}
		}

		if (attachFailed || DetourTransactionCommit() != NO_ERROR)
			break;

		result = true;
	} while (0);

	CloseAllHandle();
	m_hookInfos.clear();
	return result;
}

bool HookMgr::UnhookAll() {
	if (m_unhookInfos.empty())
		return true;
	bool result = false;
	do {

		if (DetourTransactionBegin() != NO_ERROR)
			break;
		if (!UpdateAllThread())
			break;

		bool detachFailed = false;
		MockInfoVec::iterator it = m_unhookInfos.begin();
		for (; !detachFailed && it != m_unhookInfos.end(); ++it) {
			ASSERT((*it)->m_hooked);
			(*it)->m_hooked = false;
			if (DetourDetach(&((*it)->m_newSrcAddr), (*it)->m_dstAddr) != NO_ERROR)
				detachFailed = true;
		}

		if (detachFailed || DetourTransactionCommit() != NO_ERROR)
			break;

		it = m_unhookInfos.begin();
		for (; it != m_unhookInfos.end(); ++it) {
			// ��һ��bug:��ĳ�̵߳�eip����MockFunction�����ڱ�����ʱ��Unhook������Ȼԭ����jmpָ���Ѿ����Ļ�
			// ����MockFunction����Ҫ����srcFunc��Ӧ��MockFuncInfo����ʱDel�Ļ��ͻᵼ���Ҳ����ˣ�
			
			// TODO WARNING:������Ҫ���û�����֤Unmockǰ���������߳��ٵ��������mock�ĺ�����
			MockMgr::GetInstance()->DelMockFuncInfo(*it);
		}

		result = true;
	} while (0);

	CloseAllHandle();
	m_unhookInfos.clear();
	return result;
}

void HookMgr::CloseAllHandle() {
	std::vector<HANDLE>::iterator it = m_threads.begin();
	for (; it != m_threads.end(); ++it) {
		::CloseHandle(*it);
	}
	m_threads.clear();
}

}