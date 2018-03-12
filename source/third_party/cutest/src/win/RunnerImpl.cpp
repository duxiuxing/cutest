#include "RunnerImpl.h"

#include "gtest/gtest.h"
#include <set>

CUTEST_NS_BEGIN

unsigned long long
Runner::tickCount()
{
  static LARGE_INTEGER s_ticksPerSecond = { 0 };
  LARGE_INTEGER tick;
  if ( !s_ticksPerSecond.QuadPart )
  {
    ::QueryPerformanceFrequency( &s_ticksPerSecond );
  }
  ::QueryPerformanceCounter( &tick );
  LONGLONG seconds = tick.QuadPart / s_ticksPerSecond.QuadPart;
  LONGLONG leftPart = tick.QuadPart - ( s_ticksPerSecond.QuadPart * seconds );
  LONGLONG millSeconds = leftPart * 1000 / s_ticksPerSecond.QuadPart;
  return ( unsigned long long )seconds * 1000 + millSeconds;
}

thread_id
Runner::currentThreadId()
{
  return ::GetCurrentThreadId();
}

thread_id
Runner::mainThreadId()
{
  return RunnerImpl::s_mainThreadId;
}

Runner *
Runner::instance()
{
  static RunnerImpl s_mainTestRunner;
  return &s_mainTestRunner;
}

HWND RunnerImpl::s_messageWindow = NULL;
std::set<Runnable *> RunnerImpl::s_autoDeleteRunnables;
thread_id RunnerImpl::s_mainThreadId = 0;

RunnerImpl::RunnerImpl()
{
  testing::InitGoogleTest( &__argc, __wargv );
  m_listenerManager.add( &m_testProgressLogger );

  // Register message window class.
  WNDCLASSEX wcx;
  ::ZeroMemory( &wcx, sizeof( WNDCLASSEX ) );
  wcx.cbSize = sizeof( WNDCLASSEX );
  wcx.lpfnWndProc = messageWindowProc;
  wcx.hInstance = ::GetModuleHandle( NULL );
  wcx.lpszClassName = L"CPPUNIT_MOD_MAIN_TEST_RUNNER";
  ::RegisterClassEx( &wcx );

  // Create a message window.
  s_messageWindow = ::CreateWindow(
                      wcx.lpszClassName,  // name of window class
                      NULL,               // title-bar string
                      0,                  // top-level window
                      0,                  // default horizontal position
                      0,                  // default vertical position
                      0,                  // default width
                      0,                  // default height
                      HWND_MESSAGE,       // message window
                      NULL,               // use class menu
                      wcx.hInstance,      // handle to application instance
                      NULL );             // no window-creation data

  // 通过这个异步方法给s_mainThreadId赋值
  asyncRunOnMainThread( this, false );
}

RunnerImpl::~RunnerImpl()
{
  if ( s_messageWindow )
  {
    ::DestroyWindow( s_messageWindow );
    s_messageWindow = NULL;
  }

  std::set<Runnable *>::iterator it = s_autoDeleteRunnables.begin();
  while ( it != s_autoDeleteRunnables.end() )
  {
    delete *it;
    ++it;
  }
}

void
RunnerImpl::asyncRunOnMainThread( Runnable *runnable, bool is_auto_delete )
{
  ::PostMessage( s_messageWindow, WM_RUN, ( WPARAM )runnable, is_auto_delete );
}

void
RunnerImpl::delayRunOnMainThread( unsigned int delay_ms, Runnable *runnable, bool is_auto_delete )
{
  if ( is_auto_delete )
  {
    ::PostMessage( s_messageWindow, WM_DELAY_RUN_AUTO_DELETE, ( WPARAM )runnable, delay_ms );
  }
  else
  {
    ::PostMessage( s_messageWindow, WM_DELAY_RUN, ( WPARAM )runnable, delay_ms );
  }
}

LRESULT CALLBACK
RunnerImpl::messageWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
  LRESULT ret = 0;
  switch ( Msg )
  {
  case WM_RUN:
    {
      Runnable *runnable = ( Runnable * )wParam;
      runnable->run();
      if ( lParam )
      {
        delete runnable;
      }
    }
    break;
  case WM_DELAY_RUN_AUTO_DELETE:
    {
      RunnerImpl::s_autoDeleteRunnables.insert( ( Runnable * )wParam );
      ::SetTimer( s_messageWindow, ( UINT_PTR )wParam, ( UINT )lParam, &RunnerImpl::onTimer4DelayRunAutoDelete );
    }
    break;
  case WM_DELAY_RUN:
    {
      ::SetTimer( s_messageWindow, ( UINT_PTR )wParam, ( UINT )lParam, &RunnerImpl::onTimer4DelayRun );
    }
    break;
  default:
    {
      ret = DefWindowProc( hWnd, Msg, wParam, lParam );
    }
    break;
  }
  return ret;
}

VOID CALLBACK
RunnerImpl::onTimer4DelayRunAutoDelete( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
  ::KillTimer( hwnd, idEvent );

  Runnable *runnable = ( Runnable * )idEvent;
  RunnerImpl::s_autoDeleteRunnables.erase( runnable );
  runnable->run();
  delete runnable;
}

VOID CALLBACK
RunnerImpl::onTimer4DelayRun( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
  ::KillTimer( hwnd, idEvent );

  Runnable *runnable = ( Runnable * )idEvent;
  runnable->run();
}

void
RunnerImpl::run()
{
  RunnerImpl::s_mainThreadId = Runner::currentThreadId();
}

CUTEST_NS_END
