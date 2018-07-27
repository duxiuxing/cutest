#pragma once
#include "GMockPlus.h"
#include <set>

namespace testing {

class MockMgr {
public:
	static MockMgr* GetInstance();
	~MockMgr() {}

	void AddMockFuncInfo(GMockPlus* info);
	void DelMockFuncInfo(GMockPlus* info);
	GMockPlus* QueryMockInfo(void* srcAddr);

private:
	MockMgr() {}

	typedef std::set<GMockPlus*> MockInfoSet;
private:
	MockInfoSet m_mockInfoSet;
};

}