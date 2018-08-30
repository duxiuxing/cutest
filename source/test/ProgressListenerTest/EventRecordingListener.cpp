#include "EventRecordingListener.h"
#include <vector>

extern std::vector<std::string>* g_events;
extern int g_indent;

std::string GetIndentString() {
  int indent = g_indent;
  std::string indentString;
  while (indent--) {
    indentString.append("--");
  }
  return indentString;
}

EventRecordingListener::EventRecordingListener(const char* prefix) {
  this->prefix = prefix;
}

void EventRecordingListener::onRunnerStart(CPPUNIT_NS::Test* test) {
  ++g_indent;
  g_events->push_back(GetFullMethodName(test, "onRunnerStart"));
}

void EventRecordingListener::onRunnerEnd(CPPUNIT_NS::Test* test, unsigned int elapsed_ms) {
  g_events->push_back(GetFullMethodName(test, "onRunnerEnd"));
  --g_indent;
}

void EventRecordingListener::onSuiteStart(CPPUNIT_NS::Test* suite) {
  ++g_indent;
  g_events->push_back(GetFullMethodName(suite, "onSuiteStart"));
}

void EventRecordingListener::onSuiteEnd(CPPUNIT_NS::Test* suite, unsigned int elapsed_ms) {
  g_events->push_back(GetFullMethodName(suite, "onSuiteEnd"));
  --g_indent;
}

void EventRecordingListener::onTestStart(CPPUNIT_NS::Test* test) {
  ++g_indent;
  g_events->push_back(GetFullMethodName(test, "onTestStart"));
}

void EventRecordingListener::onTestEnd(
  CPPUNIT_NS::Test* test,
  unsigned int error_count,
  unsigned int failure_count,
  unsigned int elapsed_ms) {
  g_events->push_back(GetFullMethodName(test, "onTestEnd"));
  --g_indent;
}

std::string EventRecordingListener::GetFullMethodName(CPPUNIT_NS::Test* test, const char* name) {
  return GetIndentString() + this->prefix + "." + test->getName() + "." + name;
}
