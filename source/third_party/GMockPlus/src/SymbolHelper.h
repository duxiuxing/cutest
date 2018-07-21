#pragma once
#include <map>
#include <string>
#include <Windows.h>
#include "GMockPlus.h"

namespace testing {

struct DllNameLess {
	bool operator()(const char* name1, const char* name2) const {
		return _stricmp(name1, name2) < 0;
	}

	bool operator()(const std::string& name1, const std::string& name2) const {
		return operator()(name1.c_str(), name2.c_str());
	}
};

class SymbolHelper {
public:
	typedef std::map<std::string, void*>							SymbolInfoMap;
	typedef std::map<std::string, SymbolInfoMap, DllNameLess>		ModuleInfoMap;
	typedef std::map<std::string, HMODULE, DllNameLess>				ModuleHandleMap;

	static SymbolHelper* GetInstance();
	~SymbolHelper();
	bool SetSymSearchPath(const wchar_t* utf16Path);
	void* GetSymbolAddr(const char* moduleName, const char* symbolName);
	void QueryOverloadFuncNameSet(const char* moduleName, const char* symbolName, OverloadFuncNameSet& funcMap);

private:
	void Init();

private:
	SymbolHelper() {}

	ModuleInfoMap		m_moduleInfoMap;
	ModuleHandleMap		m_moduleHdlMap;
};

}
