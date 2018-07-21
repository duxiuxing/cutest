#include "cutest/ExplicitEndTest.h"

#include "cutest/Event.h"
#include "cutest/Runner.h"

CUTEST_NS_BEGIN

ExplicitEndTest::ExplicitEndTest()
  : event( NULL )
{}

void
ExplicitEndTest::setEvent( CUTEST_NS::Event *event )
{
  this->event = event;
}

void
ExplicitEndTest::endTest()
{
  if ( isTestEnd() )
  {
    return;  // 防止重入，CUTEST_NS::Runner::Stop()/超时/延迟/异步调用逻辑都可能调用EndTest()
  }

  CUTEST_NS::Runner::instance()->unregisterExplicitEndTest( this );
  if ( this->event )
  {
    this->event->post();
    this->event = NULL;
  }
}

bool
ExplicitEndTest::isTestEnd()
{
  return ( this->event == NULL ) ? true : false;
}

CUTEST_NS_END
