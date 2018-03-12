#include "AutoEndTest.h"

#include "cutest/ManualEndTest.h"
#include "cutest/Runner.h"
#include "gtest/gtest.h"

CUTEST_NS_BEGIN

TestTimeoutCounter::TestTimeoutCounter( ManualEndTest *test )
  : m_test( test )
  , m_callback( NULL )
  , m_timeoutMs( 0 )
  , m_startMs( 0 )
{}

void
TestTimeoutCounter::start( unsigned int timeout_ms, Callback *callback )
{
  if ( timeout_ms )
  {
    m_callback = callback;
    m_timeoutMs = timeout_ms;
    m_startMs = Runner::tickCount();
    Runner::instance()->delayRunOnMainThread( timeout_ms, this, false );
  }
  else
  {
    // 超时时间为0表示无限等待，这种Test不需要启动Timer
    delete this;
  }
}

void
TestTimeoutCounter::run()
{
  // 计算ManualEndTest当前的执行时长
  unsigned int elapsedMs = ( unsigned int )( Runner::tickCount() - m_startMs );
  if ( elapsedMs < m_timeoutMs )
  {
    /*
    超时补偿逻辑：
    Timer存在误差，比如Runner::instance()->delayRunOnMainThread(1000, ...)指定的是1000ms，
    但有可能在980ms左右就调用TestTimeoutCounter::run()了，为了避免因误差导致ManualEndTest被提前结束，
    此处增加一个100ms的补偿逻辑。
    */
    Runner::instance()->delayRunOnMainThread( 100, this, false );
  }
  else
  {
    m_callback->onTimeout( m_test, this );
    delete this;
  }
}

void
TestTimeoutCounter::addFailure()
{
  // 计算ManualEndTest当前的执行时长
  unsigned int elapsedMs = ( unsigned int )( Runner::tickCount() - m_startMs );

  if ( elapsedMs < m_timeoutMs )
  {
    // ManualEndTest还没超时就被自动结束了？需要Review AutoEndTest的设计。
    EXPECT_GE( elapsedMs, m_timeoutMs ) << "AutoEndTest early end the TestCase!";
  }
  else
  {
    EXPECT_LE( elapsedMs, m_timeoutMs ) << "ManualEndTest Timeout!";
  }
}

AutoEndTest::AutoEndTest()
  : m_test( NULL )
  , m_counter( NULL )
{}

void
AutoEndTest::check( ManualEndTest *test, unsigned int timeout_ms )
{
  if ( test && timeout_ms )
  {
    m_test = test;
    m_counter = new TestTimeoutCounter( test );
    m_counter->start( timeout_ms, this );
  }
  else
  {
    m_test = NULL;
    m_counter = NULL;
  }
}

void
AutoEndTest::cancel()
{
  m_test = NULL;
  m_counter = NULL;
}

void
AutoEndTest::onTimeout( ManualEndTest *test, TestTimeoutCounter *counter )
{
  if ( m_test == test && m_counter == counter )
  {
    counter->addFailure();
    m_test->endTest();
  }
}

CUTEST_NS_END
