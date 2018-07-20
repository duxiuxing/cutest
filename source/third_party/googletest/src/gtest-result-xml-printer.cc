#ifdef _CUTEST

#include "gtest/internal/gtest-result-xml-printer.h"

#include <cppunit/TestFailure.h>
#include "cutest/Runner.h"

namespace testing {
namespace internal {

// Creates a new TestResultXmlPrinter.
TestResultXmlPrinter::TestResultXmlPrinter(const char* file_path)
  : _startTestRunMs(0)
  , _failedTestCases(0)
  , _filePath(file_path) {
  if (_filePath.c_str() == NULL || _filePath.empty()) {
    fprintf(stderr, "XML output file may not be null\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
}

TestResultXmlPrinter::~TestResultXmlPrinter() {
  TestSuiteInfoList::iterator it = _testSuiteInfos.begin();
  while (it != _testSuiteInfos.end()) {
    delete (*it);
    ++it;
  }
}

void TestResultXmlPrinter::onRunnerStart(CPPUNIT_NS::Test* test) {
  _startTestRunMs = GetTimeInMillis();
  _failedTestCases = 0;

  TestSuiteInfo* info = new TestSuiteInfo;
  info->suite = test;
  _testSuiteInfos.push_back(info);
}

// Called after the unit test ends.
void TestResultXmlPrinter::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms) {
  if (_testSuiteInfos.back()->suite == test) {
    _testSuiteInfos.back()->elapsedMs = elapsed_ms;
  }

  FILE* xmlout = NULL;
  FilePath filePath(_filePath);
  FilePath dirPath(filePath.RemoveFileName());

  if (dirPath.CreateDirectoriesRecursively()) {
    xmlout = posix::FOpen(_filePath.c_str(), "w");
  }
  if (xmlout == NULL) {
    fprintf(stderr,
            "Unable to open file \"%s\"\n",
            _filePath.c_str());
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  std::stringstream stream;
  printXmlTestSuites(&stream, test, elapsed_ms);
  fprintf(xmlout, "%s", StringStreamToString(&stream).c_str());
  fclose(xmlout);
}

void TestResultXmlPrinter::onSuiteStart(CPPUNIT_NS::Test* suite) {
  TestSuiteInfo* info = new TestSuiteInfo;
  info->suite = suite;
  _testSuiteInfos.push_back(info);
}

void TestResultXmlPrinter::onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int elapsed_ms) {
  _testSuiteInfos.back()->elapsedMs = elapsed_ms;
}

void TestResultXmlPrinter::onTestStart(CPPUNIT_NS::Test* test) {
  TestCaseInfo* info = new TestCaseInfo;
  info->test = test;
  _testSuiteInfos.back()->testCaseInfos.push_back(info);
}

void TestResultXmlPrinter::onFailureAdd(unsigned int index, const CPPUNIT_NS::TestFailure& failure) {
  // 记录每个TestFailure的index
  _testSuiteInfos.back()->testCaseInfos.back()->failureIndexs.push_back(index);
}

void TestResultXmlPrinter::onTestEnd(
  CPPUNIT_NS::Test* test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms) {
  if (error_count || failure_count) {
    ++_failedTestCases; // 所有失败的用例数
    _testSuiteInfos.back()->failedTestCases += 1; // 当前Suite的失败的用例数
  }
  _testSuiteInfos.back()->testCaseInfos.back()->elapsedMs = elapsed_ms;
}

// Returns an XML-escaped copy of the input string str.  If is_attribute
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
std::string TestResultXmlPrinter::escapeXml(
  const std::string& str, bool is_attribute) {
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
      if (is_attribute) {
        m << "&apos;";
      } else {
        m << '\'';
      }
      break;
    case '"':
      if (is_attribute) {
        m << "&quot;";
      } else {
        m << '"';
      }
      break;
    default:
      if (isValidXmlCharacter(ch)) {
        if (is_attribute && isNormalizableWhitespace(ch))
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
std::string TestResultXmlPrinter::removeInvalidXmlCharacters(
  const std::string& str) {
  std::string output;
  output.reserve(str.size());
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    if (isValidXmlCharacter(*it)) {
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
void TestResultXmlPrinter::outputXmlCDataSection(::std::ostream* stream,
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

void TestResultXmlPrinter::outputXmlAttribute(
  std::ostream* stream,
  const std::string& element_name,
  const std::string& name,
  const std::string& value) {
  const std::vector<std::string>& allowed_names =
    GetReservedAttributesForElement(element_name);

  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
               allowed_names.end())
      << "Attribute " << name << " is not allowed for element <" << element_name
      << ">.";

  *stream << " " << name << "=\"" << escapeXmlAttribute(value) << "\"";
}

// Prints an XML representation of a TestInfo object.
// TODO(wan): There is also value in printing properties with the plain printer.
void TestResultXmlPrinter::outputXmlTestCase(
  ::std::ostream* stream,
  const char* test_case_name,
  TestCaseInfo* test_case_info) {
  const std::string kTestcase = "testcase";

  // Test方法的名称
  *stream << "    <testcase";
  std::string name = test_case_info->test->getName();
  // 将Test名字中Suite::的部分精简掉
  // 比如："ExampleTestCase::testAdd"精简为"testAdd"
  std::string prefix = test_case_name;
  prefix += "::";
  std::string::size_type pos = name.find(prefix);
  if (pos != std::string::npos) {
    name.replace(pos, prefix.size(), "");
  }
  outputXmlAttribute(stream, kTestcase, "name", name);

  // if (test_info.value_param() != NULL) {
  //    OutputXmlAttribute(stream, kTestcase, "value_param",
  //                       test_info.value_param());
  // }
  // if (test_info.type_param() != NULL)
  //    OutputXmlAttribute(stream, kTestcase, "type_param", test_info.type_param());

  outputXmlAttribute(stream, kTestcase, "status", "run");

  outputXmlAttribute(stream, kTestcase, "time",
                     FormatTimeInMillisAsSeconds(test_case_info->elapsedMs));

  outputXmlAttribute(stream, kTestcase, "classname", test_case_name);
  // *stream << TestPropertiesAsXmlAttributes(result);

  int failures = 0;
  for (std::vector<unsigned int>::size_type i = 0; i < test_case_info->failureIndexs.size(); ++i) {
    const CPPUNIT_NS::TestFailure* testFailure =
      CUTEST_NS::Runner::instance()->failureAt(test_case_info->failureIndexs[i]);

    if (++failures == 1) {
      *stream << ">\n";
    }

    const string location = internal::FormatCompilerIndependentFileLocation(
                              testFailure->sourceLine().fileName().c_str(),
                              testFailure->sourceLine().lineNumber());
    const string summary = location + "\n" + testFailure->thrownException()->what();
    *stream << "      <failure message=\""
            << escapeXmlAttribute(summary.c_str())
            << "\" type=\"\">";
    const string detail = location + "\n" + testFailure->thrownException()->what();
    outputXmlCDataSection(stream, removeInvalidXmlCharacters(detail).c_str());
    *stream << "</failure>\n";
  }

  if (failures == 0) {
    *stream << " />\n";
  } else {
    *stream << "    </testcase>\n";
  }
}

// Prints an XML representation of a TestSuiteInfo object
void TestResultXmlPrinter::outputXmlTestSuite(
  std::ostream* stream,
  TestSuiteInfo* test_suite_info) {
  const std::string kTestsuite = "testsuite";
  *stream << "  <" << kTestsuite;

  // TestCase的名称
  outputXmlAttribute(stream, kTestsuite, "name", test_suite_info->name());

  // TestCase下的用例数
  outputXmlAttribute(stream, kTestsuite, "tests",
                     StreamableToString(test_suite_info->suite->countTestCases()));

  // TestCase下失败的用例数
  outputXmlAttribute(stream, kTestsuite, "failures",
                     StreamableToString(test_suite_info->failedTestCases));

  // 预留字段，目前总是填0
  outputXmlAttribute(
    stream, kTestsuite, "disabled",
    StreamableToString(0));

  // 预留字段，目前总是填0
  outputXmlAttribute(stream, kTestsuite, "errors", "0");

  // 当前TestCase的总耗时，单位：秒
  outputXmlAttribute(stream, kTestsuite, "time",
                     FormatTimeInMillisAsSeconds(test_suite_info->elapsedMs));

  // *stream << TestPropertiesAsXmlAttributes(test_case.ad_hoc_test_result());
  *stream << ">\n";

  TestCaseInfoList::iterator it = test_suite_info->testCaseInfos.begin();
  while (it != test_suite_info->testCaseInfos.end()) {
    outputXmlTestCase(stream, test_suite_info->name().c_str(), *it);
    ++it;
  }

  *stream << "  </" << kTestsuite << ">\n";
}

// Prints an XML summary of unit_test to output stream out.
void TestResultXmlPrinter::printXmlTestSuites(
  std::ostream* stream,
  CPPUNIT_NS::Test* test,
  unsigned int elapsed_ms) {
  const std::string kTestsuites = "testsuites";

  *stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  *stream << "<" << kTestsuites;

  // 总的测试用例数
  outputXmlAttribute(stream, kTestsuites, "tests",
                     StreamableToString(test->countTestCases()));


  // 失败的测试用例数
  outputXmlAttribute(stream, kTestsuites, "failures",
                     StreamableToString(_failedTestCases));

  // 预留字段，目前总是填0
  outputXmlAttribute(
    stream, kTestsuites, "disabled",
    StreamableToString(0));

  // 预留字段，目前总是填0
  outputXmlAttribute(stream, kTestsuites, "errors", "0");

  // 启动本次测试的时刻
  outputXmlAttribute(
    stream, kTestsuites, "timestamp",
    FormatEpochTimeInMillisAsIso8601(_startTestRunMs));

  // 所有测试的总耗时，单位：秒
  outputXmlAttribute(stream, kTestsuites, "time",
                     FormatTimeInMillisAsSeconds(elapsed_ms));

  // if (GTEST_FLAG(shuffle)) {
  //     OutputXmlAttribute(stream, kTestsuites, "random_seed",
  //          StreamableToString(unit_test.random_seed()));
  // }

  // *stream << TestPropertiesAsXmlAttributes(unit_test.ad_hoc_test_result());

  outputXmlAttribute(stream, kTestsuites, "name", test->getName());
  *stream << ">\n";

  TestSuiteInfoList::iterator it = _testSuiteInfos.begin();
  while (it != _testSuiteInfos.end()) {
    if ((*it)->testCaseInfos.size()) {
      outputXmlTestSuite(stream, *it);
    }
    ++it;
  }

  *stream << "</" << kTestsuites << ">\n";
}

} // namespace internal
} // namespace testing

#endif // #ifdef _CUTEST
