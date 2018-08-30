#pragma once

#include "../Logger.h"
#include "../RunnerBase.h"

#include "cutest/Helper.h"

#include <set>
#include <wtypes.h>

CUTEST_NS_BEGIN

class RunnerImpl
  : public RunnerBase
  , public Runnable
{
  friend class Runner;
  friend thread_id CUTEST_NS::mainThreadId();

public:
  RunnerImpl();
  virtual ~RunnerImpl();

protected:
  Logger test_progress_logger;

public:
  // Runner的接口实现
  virtual void asyncRunOnMainThread( Runnable *runnable, bool is_auto_delete );
  virtual void delayRunOnMainThread( unsigned int delay_ms, Runnable *runnable, bool is_auto_delete );

  virtual void waitUntilAllTestEnd();

protected:
  enum
  {
    WM_RUN = WM_USER + 1,
    WM_DELAY_RUN_AUTO_DELETE,
    WM_DELAY_RUN,
  };
  static HWND message_window;
  static LRESULT CALLBACK messageWindowProc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam );

  static VOID CALLBACK onTimer4DelayRunAutoDelete( HWND wnd, UINT msg, UINT_PTR id_event, DWORD elapse_ms );
  static VOID CALLBACK onTimer4DelayRun( HWND wnd, UINT uMsg, UINT_PTR id_event, DWORD elapse_ms );

  static std::set<Runnable *> auto_delete_runnables; // 用于缓存delayRunOnMainThread()中尚未执行且需要auto delete的Runnable对象
  static thread_id main_thread_id;

  // 实现Runnable::run()
  virtual void run();
};

CUTEST_NS_END
