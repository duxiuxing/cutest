#pragma once

#include "gtest/gtest-export.h"
#include "cutest/ProgressListener.h"

#include <list>
#include <string>
#include <vector>

namespace testing {
namespace internal {

class TestResultXmlPrinter
  : public CUTEST_NS::ProgressListener {
 public:
  TestResultXmlPrinter(const char* xmlFilePath);
  virtual ~TestResultXmlPrinter();

  //////////////////////////////////////////////////////////////////////////
  // 重载TestProgressListener的成员方法
  virtual void onRunnerStart(CPPUNIT_NS::Test* test);
  virtual void onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed);

  virtual void onSuiteStart(CPPUNIT_NS::Test* suite);
  virtual void onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed);

  virtual void onTestStart(CPPUNIT_NS::Test* test);
  virtual void onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure);
  virtual void onTestEnd(
    CPPUNIT_NS::Test* test,
    unsigned int errorCount,
    unsigned int failureCount,
    unsigned int msElapsed);
  //////////////////////////////////////////////////////////////////////////

 protected:
  TimeInMillis msStartTestRun; // 在StartTestRun()中记录本次测试启动的时刻
  unsigned int failedTestCaseCount; // 所有失败的用例数

  struct TestCaseInfo {
    TestCaseInfo()
      : test(NULL)
      , msElapsed(0) {
    }

    CPPUNIT_NS::Test* test;
    unsigned int msElapsed;
    std::vector<unsigned int> failureIndexs;
  };
  typedef std::list<TestCaseInfo*> TestCaseInfoList;

  struct TestSuiteInfo {
    TestSuiteInfo()
      : suite(NULL)
      , elapsedMs(0)
      , failedTestCases(0) {
    }

    ~TestSuiteInfo() {
      TestCaseInfoList::iterator it = testCaseInfos.begin();
      while (it != testCaseInfos.end()) {
        delete (*it);
        ++it;
      }
    }

    const std::string& name() {
      if (!suiteName.empty()) {
        return suiteName;
      }

      std::string wholeName = suite->getName();
      std::string::size_type pos = wholeName.find(".");
      if (std::string::npos == pos) {
        suiteName = wholeName;
      } else {
        suiteName = wholeName.substr(0, pos);
      }

      return suiteName;
    }

    CPPUNIT_NS::Test* suite;
    std::string suiteName;
    unsigned int elapsedMs;
    unsigned int failedTestCases;
    TestCaseInfoList testCaseInfos;
  };
  typedef std::list<TestSuiteInfo*> TestSuiteInfoList;

  TestSuiteInfoList testSuiteInfos;

 private:
  // Is c a whitespace character that is normalized to a space character
  // when it appears in an XML attribute value?
  static bool isNormalizableWhitespace(char c) {
    return c == 0x9 || c == 0xA || c == 0xD;
  }

  // May c appear in a well-formed XML document?
  static bool isValidXmlCharacter(char c) {
    return isNormalizableWhitespace(c) || c >= 0x20;
  }

  // Returns an XML-escaped copy of the input string str.  If
  // isAttribute is true, the text is meant to appear as an attribute
  // value, and normalizable whitespace is preserved by replacing it
  // with character references.
  static std::string escapeXml(const std::string& str, bool isAttribute);

  // Returns the given string with all characters invalid in XML removed.
  static std::string removeInvalidXmlCharacters(const std::string& str);

  // Convenience wrapper around EscapeXml when str is an attribute value.
  static std::string escapeXmlAttribute(const std::string& str) {
    return escapeXml(str, true);
  }

  // Verifies that the given attribute belongs to the given element and
  // streams the attribute as XML.
  static void outputXmlAttribute(std::ostream* stream,
                                 const std::string& element_name,
                                 const std::string& name,
                                 const std::string& value);

  // Streams an XML CDATA section, escaping invalid CDATA sequences as needed.
  static void outputXmlCDataSection(::std::ostream* stream, const char* data);

  // Streams an XML representation of a TestCaseInfo object.
  void outputXmlTestCase(::std::ostream* stream,
                         const char* testCaseName,
                         TestCaseInfo* testCaseInfo);

  // Prints an XML representation of a TestSuiteInfo object
  void outputXmlTestSuite(::std::ostream* stream, TestSuiteInfo* testSuiteInfo);

  // Prints an XML summary of unit_test to output stream out.
  void printXmlTestSuites(::std::ostream* stream, CPPUNIT_NS::Test* test, unsigned int msElapsed);

  // The output file.
  const std::string xmlFilePath;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(TestResultXmlPrinter);
};

} // namespace internal
} // namespace testing
