#pragma once

#include "gtest/gtest-export.h"
#include "cutest/Listener.h"

#include <list>
#include <string>
#include <vector>

namespace testing { // _CUTEST_IMPL
namespace internal {

class TestResultXmlPrinter
  : public CUTEST_NS::Listener {
 public:
  TestResultXmlPrinter(const char* xmlFilePath);
  virtual ~TestResultXmlPrinter();

  //////////////////////////////////////////////////////////////////////////
  // 重载CUTEST_NS::Listener的方法
  virtual void OnRunnerStart(CPPUNIT_NS::Test* test) override;
  virtual void OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) override;

  virtual void OnSuiteStart(CPPUNIT_NS::Test* suite) override;
  virtual void OnSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) override;

  virtual void OnTestStart(CPPUNIT_NS::Test* test) override;
  virtual void OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) override;
  virtual void OnTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int errorCount,
    unsigned int failureCount,
    unsigned int msElapsed);
  //////////////////////////////////////////////////////////////////////////

 protected:
  TimeInMillis m_msStartTestRun; // 在StartTestRun()中记录本次测试启动的时刻
  unsigned int m_failedTestCaseCount; // 所有失败的用例数

  struct TestCaseInfo {
    TestCaseInfo()
      : Test(NULL)
      , MsElapsed(0) {
    }

    CPPUNIT_NS::Test* Test;
    unsigned int MsElapsed;
    std::vector<unsigned int> FailureIndexs;
  };
  typedef std::list<TestCaseInfo*> TestCaseInfoList;

  struct TestSuiteInfo {
    TestSuiteInfo()
      : Suite(NULL)
      , MsElapsed(0)
      , FailedTestCaseCount(0) {
    }

    ~TestSuiteInfo() {
      TestCaseInfoList::iterator it = TestCaseInfos.begin();
      while (it != TestCaseInfos.end()) {
        delete (*it);
        ++it;
      }
    }

    const std::string& Name() {
      if (!m_name.empty()) {
        return m_name;
      }

      std::string wholeName = Suite->getName();
      size_t pos = wholeName.find(".");
      if (std::string::npos == pos) {
        m_name = wholeName;
      } else {
        m_name = wholeName.substr(0, pos);
      }

      return m_name;
    }

    CPPUNIT_NS::Test* Suite;
    unsigned int MsElapsed;
    unsigned int FailedTestCaseCount;
    TestCaseInfoList TestCaseInfos;
   protected:
    std::string m_name;
  };
  typedef std::list<TestSuiteInfo*> TestSuiteInfoList;

  TestSuiteInfoList m_testSuiteInfos;

 private:
  // Is c a whitespace character that is normalized to a space character
  // when it appears in an XML attribute value?
  static bool IsNormalizableWhitespace(char c) {
    return c == 0x9 || c == 0xA || c == 0xD;
  }

  // May c appear in a well-formed XML document?
  static bool IsValidXmlCharacter(char c) {
    return IsNormalizableWhitespace(c) || c >= 0x20;
  }

  // Returns an XML-escaped copy of the input string str.  If
  // isAttribute is true, the text is meant to appear as an attribute
  // value, and normalizable whitespace is preserved by replacing it
  // with character references.
  static std::string EscapeXml(const std::string& str, bool isAttribute);

  // Returns the given string with all characters invalid in XML removed.
  static std::string RemoveInvalidXmlCharacters(const std::string& str);

  // Convenience wrapper around EscapeXml when str is an attribute value.
  static std::string EscapeXmlAttribute(const std::string& str) {
    return EscapeXml(str, true);
  }

  // Verifies that the given attribute belongs to the given element and
  // streams the attribute as XML.
  static void OutputXmlAttribute(std::ostream* stream,
                                 const std::string& elementName,
                                 const std::string& name,
                                 const std::string& value);

  // Streams an XML CDATA section, escaping invalid CDATA sequences as needed.
  static void OutputXmlCDataSection(::std::ostream* stream, const char* data);

  // Streams an XML representation of a TestCaseInfo object.
  void OutputXmlTestCase(::std::ostream* stream,
                         const char* testCaseName,
                         TestCaseInfo* testCaseInfo);

  // Prints an XML representation of a TestSuiteInfo object
  void OutputXmlTestSuite(::std::ostream* stream, TestSuiteInfo* testSuiteInfo);

  // Prints an XML summary of unit_test to output stream out.
  void PrintXmlTestSuites(::std::ostream* stream, CPPUNIT_NS::Test* test, unsigned int msElapsed);

  // The output file.
  const std::string m_xmlFilePath;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestResultXmlPrinter);
};

} // namespace internal
} // namespace testing // _CUTEST_IMPL
