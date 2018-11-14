#include "gtest/internal/gtest-result-xml-printer.h"

#include <cppunit/TestFailure.h>
#include "cutest/Runner.h"

namespace testing { // _CUTEST_IMPL
namespace internal {

// Creates a new TestResultXmlPrinter.
TestResultXmlPrinter::TestResultXmlPrinter(const char* xmlFilePath)
  : m_msStartTestRun(0)
  , m_failedTestCaseCount(0)
  , m_xmlFilePath(xmlFilePath) {
  if (m_xmlFilePath.c_str() == NULL || m_xmlFilePath.empty()) {
    fprintf(stderr, "XML output file may not be null\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
}

TestResultXmlPrinter::~TestResultXmlPrinter() {
  TestSuiteInfoList::iterator it = m_testSuiteInfos.begin();
  while (it != m_testSuiteInfos.end()) {
    delete (*it);
    ++it;
  }
}

void TestResultXmlPrinter::OnRunnerStart(CPPUNIT_NS::Test* test) {
  m_msStartTestRun = GetTimeInMillis();
  m_failedTestCaseCount = 0;

  TestSuiteInfo* info = new TestSuiteInfo;
  info->Suite = test;
  m_testSuiteInfos.push_back(info);
}

// Called after the unit test ends.
void TestResultXmlPrinter::OnRunnerEnd(CPPUNIT_NS::Test* test, unsigned int msElapsed) {
  if (m_testSuiteInfos.back()->Suite == test) {
    m_testSuiteInfos.back()->MsElapsed = msElapsed;
  }

  FILE* xmlout = NULL;
  FilePath filePath(m_xmlFilePath);
  FilePath dirPath(filePath.RemoveFileName());

  if (dirPath.CreateDirectoriesRecursively()) {
    xmlout = posix::FOpen(m_xmlFilePath.c_str(), "w");
  }
  if (xmlout == NULL) {
    fprintf(stderr,
            "Unable to open file \"%s\"\n",
            m_xmlFilePath.c_str());
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  std::stringstream stream;
  PrintXmlTestSuites(&stream, test, msElapsed);
  fprintf(xmlout, "%s", StringStreamToString(&stream).c_str());
  fclose(xmlout);
}

void TestResultXmlPrinter::OnSuiteStart(CPPUNIT_NS::Test* suite) {
  TestSuiteInfo* info = new TestSuiteInfo;
  info->Suite = suite;
  m_testSuiteInfos.push_back(info);
}

void TestResultXmlPrinter::OnSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int msElapsed) {
  m_testSuiteInfos.back()->MsElapsed = msElapsed;
}

void TestResultXmlPrinter::OnTestStart(CPPUNIT_NS::Test* test) {
  TestCaseInfo* info = new TestCaseInfo;
  info->Test = test;
  m_testSuiteInfos.back()->TestCaseInfos.push_back(info);
}

void TestResultXmlPrinter::OnFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
  // 记录每个TestFailure的index
  m_testSuiteInfos.back()->TestCaseInfos.back()->FailureIndexs.push_back(index);
}

void TestResultXmlPrinter::OnTestEnd(
  CPPUNIT_NS::Test* test,
  unsigned int errorCount,
  unsigned int failureCount,
  unsigned int msElapsed) {
  if (errorCount || failureCount) {
    ++m_failedTestCaseCount; // 所有失败的用例数
    m_testSuiteInfos.back()->FailedTestCaseCount += 1; // 当前Suite的失败的用例数
  }
  m_testSuiteInfos.back()->TestCaseInfos.back()->MsElapsed = msElapsed;
}

// Returns an XML-escaped copy of the input string str.  If isAttribute
// is true, the text is meant to appear as an attribute value, and
// normalizable whitespace is preserved by replacing it with character
// references.
//
// Invalid XML characters in str, if any, are stripped from the output.
// It is expected that most, if not all, of the text processed by this
// module will consist of ordinary English text.
// If this module is ever modified to produce version 1.1 XML output,
// most invalid characters can be retained using character references.
// TODO(wan): It might be nice to have a minimally invasive, human-readable
// escaping scheme for invalid characters, rather than dropping them.
std::string TestResultXmlPrinter::EscapeXml(
  const std::string& str, bool isAttribute) {
  Message m;

  for (size_t i = 0; i < str.size(); ++i) {
    const char ch = str[i];
    switch (ch) {
    case '<':
      m << "&lt;";
      break;
    case '>':
      m << "&gt;";
      break;
    case '&':
      m << "&amp;";
      break;
    case '\'':
      if (isAttribute) {
        m << "&apos;";
      } else {
        m << '\'';
      }
      break;
    case '"':
      if (isAttribute) {
        m << "&quot;";
      } else {
        m << '"';
      }
      break;
    default:
      if (IsValidXmlCharacter(ch)) {
        if (isAttribute && IsNormalizableWhitespace(ch))
          m << "&#x" << String::FormatByte(static_cast<unsigned char>(ch))
            << ";";
        else {
          m << ch;
        }
      }
      break;
    }
  }

  return m.GetString();
}

// Returns the given string with all characters invalid in XML removed.
// Currently invalid characters are dropped from the string. An
// alternative is to replace them with certain characters such as . or ?.
std::string TestResultXmlPrinter::RemoveInvalidXmlCharacters(
  const std::string& str) {
  std::string output;
  output.reserve(str.size());
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    if (IsValidXmlCharacter(*it)) {
      output.push_back(*it);
    }

  return output;
}

// The following routines generate an XML representation of a UnitTest
// object.
//
// This is how Google Test concepts map to the DTD:
//
// <testsuites name="AllTests">        <-- corresponds to a UnitTest object
//   <testsuite name="testcase-name">  <-- corresponds to a TestCase object
//     <testcase name="test-name">     <-- corresponds to a TestInfo object
//       <failure message="...">...</failure>
//       <failure message="...">...</failure>
//       <failure message="...">...</failure>
//                                     <-- individual assertion failures
//     </testcase>
//   </testsuite>
// </testsuites>

// Streams an XML CDATA section, escaping invalid CDATA sequences as needed.
void TestResultXmlPrinter::OutputXmlCDataSection(::std::ostream* stream,
    const char* data) {
  const char* segment = data;
  *stream << "<![CDATA[";
  for (;;) {
    const char* const next_segment = strstr(segment, "]]>");
    if (next_segment != NULL) {
      stream->write(
        segment, static_cast<std::streamsize>(next_segment - segment));
      *stream << "]]>]]&gt;<![CDATA[";
      segment = next_segment + strlen("]]>");
    } else {
      *stream << segment;
      break;
    }
  }
  *stream << "]]>";
}

void TestResultXmlPrinter::OutputXmlAttribute(
  std::ostream* stream,
  const std::string& elementName,
  const std::string& name,
  const std::string& value) {
  const std::vector<std::string>& allowedNames =
    GetReservedAttributesForElement(elementName);

  GTEST_CHECK_(std::find(allowedNames.begin(), allowedNames.end(), name) !=
               allowedNames.end())
      << "Attribute " << name << " is not allowed for element <" << elementName
      << ">.";

  *stream << " " << name << "=\"" << EscapeXmlAttribute(value) << "\"";
}

// Prints an XML representation of a TestInfo object.
// TODO(wan): There is also value in printing properties with the plain printer.
void TestResultXmlPrinter::OutputXmlTestCase(
  ::std::ostream* stream,
  const char* testCaseName,
  TestCaseInfo* testCaseInfo) {
  const std::string kTestcase = "testcase";

  // Test方法的名称
  *stream << "    <testcase";
  std::string name = testCaseInfo->Test->getName();
  // 将Test名字中Suite.的部分精简掉
  // 比如："ExampleTestCase.testAdd"精简为"testAdd"
  std::string prefix = testCaseName;
  prefix += ".";
  std::string::size_type pos = name.find(prefix);
  if (pos != std::string::npos) {
    name.replace(pos, prefix.size(), "");
  }
  OutputXmlAttribute(stream, kTestcase, "name", name);

  // if (test_info.value_param() != NULL) {
  //    OutputXmlAttribute(stream, kTestcase, "value_param",
  //                       test_info.value_param());
  // }
  // if (test_info.type_param() != NULL)
  //    OutputXmlAttribute(stream, kTestcase, "type_param", test_info.type_param());

  OutputXmlAttribute(stream, kTestcase, "status", "run");

  OutputXmlAttribute(stream, kTestcase, "time",
                     FormatTimeInMillisAsSeconds(testCaseInfo->MsElapsed));

  OutputXmlAttribute(stream, kTestcase, "classname", testCaseName);
  // *stream << TestPropertiesAsXmlAttributes(result);

  int failures = 0;
  std::vector<unsigned int>::iterator it = testCaseInfo->FailureIndexs.begin();
  while (it != testCaseInfo->FailureIndexs.end()) {
    const CPPUNIT_NS::TestFailure* testFailure =
      CUTEST_NS::Runner::Instance()->FailureAt(static_cast<unsigned int>(*it));

    if (++failures == 1) {
      *stream << ">\n";
    }

    const string location = internal::FormatCompilerIndependentFileLocation(
                              testFailure->sourceLine().fileName().c_str(),
                              testFailure->sourceLine().lineNumber());
    const string summary = location + "\n" + testFailure->thrownException()->what();
    *stream << "      <failure message=\""
            << EscapeXmlAttribute(summary.c_str())
            << "\" type=\"\">";
    const string detail = location + "\n" + testFailure->thrownException()->what();
    OutputXmlCDataSection(stream, RemoveInvalidXmlCharacters(detail).c_str());
    *stream << "</failure>\n";

    ++it;
  }

  if (failures == 0) {
    *stream << " />\n";
  } else {
    *stream << "    </testcase>\n";
  }
}

// Prints an XML representation of a TestSuiteInfo object
void TestResultXmlPrinter::OutputXmlTestSuite(
  std::ostream* stream,
  TestSuiteInfo* testSuiteInfo) {
  const std::string kTestsuite = "testsuite";
  *stream << "  <" << kTestsuite;

  // TestCase的名称
  OutputXmlAttribute(stream, kTestsuite, "name", testSuiteInfo->Name());

  // TestCase下的用例数
  OutputXmlAttribute(stream, kTestsuite, "tests",
                     StreamableToString(testSuiteInfo->Suite->countTestCases()));

  // TestCase下失败的用例数
  OutputXmlAttribute(stream, kTestsuite, "failures",
                     StreamableToString(testSuiteInfo->FailedTestCaseCount));

  // 预留字段，目前总是填0
  OutputXmlAttribute(
    stream, kTestsuite, "disabled",
    StreamableToString(0));

  // 预留字段，目前总是填0
  OutputXmlAttribute(stream, kTestsuite, "errors", "0");

  // 当前TestCase的总耗时，单位：秒
  OutputXmlAttribute(stream, kTestsuite, "time",
                     FormatTimeInMillisAsSeconds(testSuiteInfo->MsElapsed));

  // *stream << TestPropertiesAsXmlAttributes(test_case.ad_hoc_test_result());
  *stream << ">\n";

  TestCaseInfoList::iterator it = testSuiteInfo->TestCaseInfos.begin();
  while (it != testSuiteInfo->TestCaseInfos.end()) {
    OutputXmlTestCase(stream, testSuiteInfo->Name().c_str(), *it);
    ++it;
  }

  *stream << "  </" << kTestsuite << ">\n";
}

// Prints an XML summary of unit_test to output stream out.
void TestResultXmlPrinter::PrintXmlTestSuites(
  std::ostream* stream,
  CPPUNIT_NS::Test* test,
  unsigned int msElapsed) {
  const std::string kTestsuites = "testsuites";

  *stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  *stream << "<" << kTestsuites;

  // 总的测试用例数
  OutputXmlAttribute(stream, kTestsuites, "tests",
                     StreamableToString(test->countTestCases()));


  // 失败的测试用例数
  OutputXmlAttribute(stream, kTestsuites, "failures",
                     StreamableToString(m_failedTestCaseCount));

  // 预留字段，目前总是填0
  OutputXmlAttribute(
    stream, kTestsuites, "disabled",
    StreamableToString(0));

  // 预留字段，目前总是填0
  OutputXmlAttribute(stream, kTestsuites, "errors", "0");

  // 启动本次测试的时刻
  OutputXmlAttribute(
    stream, kTestsuites, "timestamp",
    FormatEpochTimeInMillisAsIso8601(m_msStartTestRun));

  // 所有测试的总耗时，单位：秒
  OutputXmlAttribute(stream, kTestsuites, "time",
                     FormatTimeInMillisAsSeconds(msElapsed));

  // if (GTEST_FLAG(shuffle)) {
  //     OutputXmlAttribute(stream, kTestsuites, "random_seed",
  //          StreamableToString(unit_test.random_seed()));
  // }

  // *stream << TestPropertiesAsXmlAttributes(unit_test.ad_hoc_test_result());

  OutputXmlAttribute(stream, kTestsuites, "name", test->getName());
  *stream << ">\n";

  TestSuiteInfoList::iterator it = m_testSuiteInfos.begin();
  while (it != m_testSuiteInfos.end()) {
    if ((*it)->TestCaseInfos.size()) {
      OutputXmlTestSuite(stream, *it);
    }
    ++it;
  }

  *stream << "</" << kTestsuites << ">\n";
}

} // namespace internal
} // namespace testing // _CUTEST_IMPL
