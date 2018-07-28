#include "SymbolHelper.h"
#include "dbghelp.h"
#include "psapi.h"

#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"psapi.lib")

#define NAME_SIZE 4096

namespace testing {

SymbolHelper::SymbolInfoMap* g_symbolInfoMap = new SymbolHelper::SymbolInfoMap();

BOOL CALLBACK EnumSymCallBack(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext) {
  char* nameOnly = new char[NAME_SIZE]();
  UnDecorateSymbolName(pSymInfo->Name, nameOnly, NAME_SIZE, UNDNAME_NAME_ONLY);
  char* fullName = new char[NAME_SIZE]();
  UnDecorateSymbolName(
    pSymInfo->Name, fullName, NAME_SIZE,
    UNDNAME_NO_FUNCTION_RETURNS
    | UNDNAME_NO_ACCESS_SPECIFIERS
    | UNDNAME_NO_MS_KEYWORDS
    | UNDNAME_NO_MEMBER_TYPE
    | UNDNAME_NO_THISTYPE
    | UNDNAME_NO_ALLOCATION_LANGUAGE
    | UNDNAME_NO_ALLOCATION_MODEL
  );

  SymbolHelper::SymbolInfoMap::iterator symbolInfoIt = g_symbolInfoMap->find(nameOnly);
  if (symbolInfoIt == g_symbolInfoMap->end()) {
    g_symbolInfoMap->insert(std::make_pair(nameOnly, (void*)pSymInfo->Address));
  } else {    // 重载函数
    symbolInfoIt->second = NULL;
  }
  if (g_symbolInfoMap->find(fullName) == g_symbolInfoMap->end()) {
    g_symbolInfoMap->insert(std::make_pair(fullName, (void*)pSymInfo->Address));
  }

  delete[] nameOnly;
  delete[] fullName;
  return TRUE;
}

SymbolHelper* SymbolHelper::GetInstance() {
  static SymbolHelper* helper = NULL;
  if (helper == NULL) {
    helper = new SymbolHelper;
    helper->Init();
  }

  return helper;
}

SymbolHelper::~SymbolHelper()
{}

void SymbolHelper::Init() {
  SymInitialize(GetCurrentProcess(), 0, FALSE);
  SymSetOptions(SYMOPT_PUBLICS_ONLY); // 这个option能够保证枚举到的symbol名是可以区分重载函数的
}

bool SymbolHelper::SetSymSearchPath(const wchar_t* utf16Path) {
  ASSERT(utf16Path && wcslen(utf16Path) > 0);
  m_moduleHdlMap.clear();
  m_moduleInfoMap.clear();
  HANDLE procHdl = GetCurrentProcess();
  SymCleanup(procHdl);
  Init();
#if _MSC_VER >= 1500
  return (SymSetSearchPathW(procHdl, utf16Path) ? true : false);
#else
  bool result = false;
  int bufferSize = WideCharToMultiByte(CP_ACP, 0, utf16Path, wcslen(utf16Path) + 1, NULL, 0, NULL, NULL);
  if (bufferSize) {
    char* ansiPath = new char[bufferSize];
    if (WideCharToMultiByte(CP_ACP, 0, utf16Path, wcslen(utf16Path) + 1, ansiPath, bufferSize, NULL, NULL)) {
      result = (SymSetSearchPath(procHdl, ansiPath) ? true : false);
    }
    delete[] ansiPath;
    ansiPath = NULL;
  }

  return result;
#endif
}

void* SymbolHelper::GetSymbolAddr(const char* moduleName, const char* symbolName) {
  ASSERT(moduleName && symbolName);
  void* result = NULL;
  bool calced = false;

  HANDLE procHandle = GetCurrentProcess();
  HMODULE module = GetModuleHandleA(moduleName);
  for (; !calced && module != NULL;) { // 若module=NULL则表示dll未加载
    ModuleInfoMap::iterator it = m_moduleInfoMap.find(moduleName);
    if (it == m_moduleInfoMap.end()) {
      ASSERT(m_moduleHdlMap.find(moduleName) == m_moduleHdlMap.end());
      MODULEINFO info = {0};
      if (GetModuleInformation(procHandle, module, &info, sizeof(info))) {

#if _MSC_VER >= 1500
        DWORD64 dllBase = SymLoadModule64(procHandle, NULL, moduleName, NULL, (DWORD64)(info.lpBaseOfDll), info.SizeOfImage);
#else
        char tempModuleName[MAX_PATH] = {0};
        strcpy_s(tempModuleName, moduleName);
        DWORD64 dllBase = SymLoadModule64(procHandle, NULL, tempModuleName, NULL, (DWORD64)(info.lpBaseOfDll), info.SizeOfImage);
#endif
        if (dllBase != 0 && SymEnumSymbols(procHandle, dllBase, 0, EnumSymCallBack, 0)) {
          // Remarks:
          // All DbgHelp functions, such as this one, are single threaded. Therefore,
          // calls from more than one thread to this function will likely result in unexpected behavior or memory corruption.
          // To avoid this, you must synchronize all concurrent calls from more than one thread to this function.
          if (!g_symbolInfoMap->empty()) {
            std::pair<ModuleInfoMap::iterator, bool> resPair = m_moduleInfoMap.insert(std::make_pair(moduleName, *g_symbolInfoMap));
            ASSERT(resPair.second);
            it = resPair.first;

            m_moduleHdlMap.insert(std::make_pair(moduleName, module));
            g_symbolInfoMap->clear();
          }
        }
      }
    }

    if (it != m_moduleInfoMap.end()) {
      ModuleHandleMap::iterator hdlIt = m_moduleHdlMap.find(moduleName);
      ASSERT(m_moduleHdlMap.find(moduleName) != m_moduleHdlMap.end());
      if (module != hdlIt->second) {  // 说明DLL被重新装载了，删掉重来！
        m_moduleInfoMap.erase(it);
        m_moduleHdlMap.erase(hdlIt);
        continue;
      }
      SymbolInfoMap::iterator symbolIt = it->second.find(symbolName);
      if (symbolIt != it->second.end()) {
        result = symbolIt->second;
      }
    }
    calced = true;
  }

  return result;
}

void SymbolHelper::QueryOverloadFuncNameSet(const char* moduleName, const char* symbolName, OverloadFuncNameSet& funcSet) {
  void* symbolAddr = GetSymbolAddr(moduleName, symbolName);
  if (symbolAddr != NULL) {
    funcSet.insert(symbolName);
  } else {
    ModuleInfoMap::iterator it = m_moduleInfoMap.find(moduleName);

    if (it != m_moduleInfoMap.end()) {
      SymbolInfoMap::iterator symbolIt = it->second.begin();
      for (; symbolIt != it->second.end(); ++symbolIt) {
        if (symbolIt->second != NULL && symbolIt->first.find(symbolName) != std::string::npos) {
          ASSERT(funcSet.find(symbolIt->first) == funcSet.end());
          funcSet.insert(symbolIt->first);
        }
      }
    }
  }
}

bool SetSymSearchPath(const wchar_t* utf16Path) {
  bool result = false;
  if (utf16Path && wcslen(utf16Path) != 0) {
    result = SymbolHelper::GetInstance()->SetSymSearchPath(utf16Path);
  }

  return result;
}

void* GetSymbolAddr(const char* moduleName, const char* symbolName) {
  if (moduleName && symbolName) {
    return SymbolHelper::GetInstance()->GetSymbolAddr(moduleName, symbolName);
  }

  return NULL;
}

void QueryOverloadFuncNameSet(const char* moduleName, const char* symbolName, OverloadFuncNameSet& funcSet) {
  funcSet.clear();
  if (moduleName && symbolName) {
    return SymbolHelper::GetInstance()->QueryOverloadFuncNameSet(moduleName, symbolName, funcSet);
  }
}

} // namespace testing {
