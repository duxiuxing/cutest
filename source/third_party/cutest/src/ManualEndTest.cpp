#include "cutest/ManualEndTest.h"

#include "cutest/Event.h"
#include "cutest/Runner.h"

CUTEST_NS_BEGIN

ManualEndTest::ManualEndTest()
  : event( NULL )
{}

void
ManualEndTest::setEvent( CUTEST_NS::Event *event_in )
{
  this->event = event_in;
}

void
ManualEndTest::endTest()
{
  if ( isTestEnd() )
  {
    return;  // 防止重入，CUTEST_NS::Runner::Stop()/超时/延迟/异步调用逻辑都可能调用EndTest()
  }

  CUTEST_NS::Runner::instance()->unregisterManualEndTest( this );
  if ( this->event )
  {
    this->event->post();
    this->event = NULL;
  }
}

bool
ManualEndTest::isTestEnd()
{
  return ( this->event == NULL ) ? true : false;
}

CUTEST_NS_END
