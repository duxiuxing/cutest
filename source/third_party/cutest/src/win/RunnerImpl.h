#pragma once

#include "../Logger.h"
#include "../RunnerBase.h"

#include <set>
#include <wtypes.h>

CUTEST_NS_BEGIN

class RunnerImpl
  : public RunnerBase
  , public Runnable
{
  friend class Runner;
public:
  RunnerImpl();
  virtual ~RunnerImpl();

protected:
  Logger m_testProgressLogger;

public:
  // Runner的接口实现
  virtual void asyncRunOnMainThread( Runnable *runnable, bool is_auto_delete );
  virtual void delayRunOnMainThread( unsigned int delay_ms, Runnable *runnable, bool is_auto_delete );

protected:
  enum
  {
    WM_RUN = WM_USER + 1,
    WM_DELAY_RUN_AUTO_DELETE,
    WM_DELAY_RUN,
  };
  static HWND s_messageWindow;
  static LRESULT CALLBACK messageWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

  static VOID CALLBACK onTimer4DelayRunAutoDelete( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
  static VOID CALLBACK onTimer4DelayRun( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

  static std::set<Runnable *> s_autoDeleteRunnables; // 用于缓存delayRunOnMainThread()中尚未执行且需要auto delete的Runnable对象
  static thread_id s_mainThreadId;

  // 实现Runnable::run()
  virtual void run();
};

CUTEST_NS_END
