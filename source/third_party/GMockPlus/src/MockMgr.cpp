#include "MockMgr.h"
#include "GMockPlus.h"

namespace testing {

MockMgr* MockMgr::GetInstance() {
	static MockMgr* mgr = NULL;
	if (mgr == NULL) {
		mgr = new MockMgr;
	}

	return mgr;
}

GMockPlus* MockMgr::QueryMockInfo(void* srcAddr) {
	ASSERT(srcAddr);
	GMockPlus* result = NULL;

	MockInfoSet::iterator it = m_mockInfoSet.begin();
	for (; it != m_mockInfoSet.end(); ++it) {
		if ((*it)->m_srcAddr == srcAddr) {
			result = *it;
			break;
		}
	}

	if (!result) {	// Debug版本通过PDB获取到的函数地址与直接写函数指针得到的地址不一样
		void* jmpSrcAddr = NULL;
		byte binArr[5] = {0};
		memcpy(binArr, srcAddr, 5);
		if (binArr[0] == 0xe9) {	// jmp
			jmpSrcAddr = (byte*)srcAddr + *(unsigned int*)(&binArr[1]) + 5;
		}
		if (jmpSrcAddr) {
			it = m_mockInfoSet.begin();
			for (; it != m_mockInfoSet.end(); ++it) {
				if ((*it)->m_srcAddr == jmpSrcAddr) {
					result = *it;
					break;
				}
			}
		}
	}

	return result;
}

void MockMgr::AddMockFuncInfo(GMockPlus* info) {
	ASSERT(m_mockInfoSet.find(info) == m_mockInfoSet.end());
	m_mockInfoSet.insert(info);
}

void MockMgr::DelMockFuncInfo(GMockPlus* info) {
	ASSERT(m_mockInfoSet.find(info) != m_mockInfoSet.end());
	m_mockInfoSet.erase(info);
}

}