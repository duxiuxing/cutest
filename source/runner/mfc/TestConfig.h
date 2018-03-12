#pragma once

#define TEST_CONFIG_FILE _T("test_config.xml")

class TestConfig
{
public:
  static TestConfig *instance();

  virtual BOOL load() PURE;
  virtual LPCTSTR title() PURE;
};
