#include <cppunit/config/SourcePrefix.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestResult.h>

#ifdef _CUTEST_IMPL
#include "src/gtest-internal-inl.h"
#include "src/runner_base.h"
#endif

CPPUNIT_NS_BEGIN


/// Default constructor
TestSuite::TestSuite( std::string name )
    : TestComposite( name )
{
  CUTEST_NS::initGoogleMock();
}


/// Destructor
TestSuite::~TestSuite()
{ 
  deleteContents(); 
}


/// Deletes all tests in the suite.
void 
TestSuite::deleteContents()
{
  int childCount = getChildTestCount();
  for ( int index =0; index < childCount; ++index )
    delete getChildTestAt( index );

  m_tests.clear();
}


/// Adds a test to the suite. 
void 
TestSuite::addTest( Test *test )
{
  if ( 0 == test->getChildTestCount() )
  {
    if ( testing::internal::UnitTestOptions::FilterMatchesTest( test->getName() ) )
    {
      m_tests.push_back( test );
    }
    else
    {
      delete test;
    }
  }
  else
  {
    m_tests.push_back( test );
  }
}


const CppUnitVector<Test *> &
TestSuite::getTests() const
{
  return m_tests;
}


int 
TestSuite::getChildTestCount() const
{
  return (int)m_tests.size();
}


Test *
TestSuite::doGetChildTestAt( int index ) const
{
  return m_tests[index];
}

class TestCaseMethodList
{
public:
  static TestCaseMethodList *instance()
  {
    static TestCaseMethodList list;
    return &list;
  }

  void RegisterSetUpMethod( const std::string &name, TestSuite::SetUpTestCaseMethod set_up_tc )
  {
    TestCaseMethodList::instance()->setUpMethods[name] = set_up_tc;
  }

  void RunSetUpTestCase( const std::string &name )
  {
    SetUpMethods::iterator it = this->setUpMethods.find( name );
    if (it != this->setUpMethods.end())
    {
      (*(it->second))();
    }
  }

  void RegisterTearDownMethod( const std::string &name, TestSuite::TearDownTestCaseMethod tear_down_tc )
  {
    TestCaseMethodList::instance()->tearDownMethods[name] = tear_down_tc;
  }

  void RunTearDownTestCase( const std::string &name )
  {
    TearDownMethods::iterator it = this->tearDownMethods.find( name );
    if (it != this->tearDownMethods.end())
    {
      (*(it->second))();
    }
  }

private:
  typedef CppUnitMap<std::string, TestSuite::SetUpTestCaseMethod, std::less<std::string> > SetUpMethods;
  SetUpMethods setUpMethods;
  
  typedef CppUnitMap<std::string, TestSuite::TearDownTestCaseMethod, std::less<std::string> > TearDownMethods;
  TearDownMethods tearDownMethods;
};

void
TestSuite::RegisterSetUpTestCase( std::string name, TestSuite::SetUpTestCaseMethod set_up_tc )
{
  TestCaseMethodList::instance()->RegisterSetUpMethod( name, set_up_tc );
}

void
TestSuite::RegisterTearDownTestCase( std::string name, TestSuite::TearDownTestCaseMethod tear_down_tc )
{
  TestCaseMethodList::instance()->RegisterTearDownMethod( name, tear_down_tc );
}

void 
TestSuite::doStartSuite( TestResult *controller )
{
  TestCaseMethodList::instance()->RunSetUpTestCase( getName() );
  TestComposite::doStartSuite( controller );
}

void 
TestSuite::doEndSuite( TestResult *controller )
{
  TestComposite::doEndSuite( controller );
  TestCaseMethodList::instance()->RunTearDownTestCase( getName() );
}

CPPUNIT_NS_END
