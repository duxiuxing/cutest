#include "cutest/ManualEndTest.h"

#include "cutest/Event.h"
#include "cutest/Runner.h"

CUTEST_NS_BEGIN

ManualEndTest::ManualEndTest()
  : m_event( NULL )
{}

void
ManualEndTest::setEvent( CUTEST_NS::Event *event )
{
  m_event = event;
}

void
ManualEndTest::endTest()
{
  if ( isTestEnd() )
  {
    return;  // 防止重入，CUTEST_NS::Runner::Stop()/超时/延迟/异步调用逻辑都可能调用EndTest()
  }

  CUTEST_NS::Runner::instance()->unregisterManualEndTest( this );
  if ( m_event )
  {
    m_event->post();
    m_event = NULL;
  }
}

bool
ManualEndTest::isTestEnd()
{
  return ( m_event == NULL ) ? true : false;
}

CUTEST_NS_END
