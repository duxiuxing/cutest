#pragma once

#include "TestConfig.h"

class TestConfigImpl : public TestConfig
{
public:
  TestConfigImpl();

  virtual BOOL load();
  virtual LPCTSTR title();

protected:
  static BOOL isFileExist( CString &file_path );
  static void loadFailedMsgBox( LPTSTR lib_name );

protected:
  CString _title;
};
