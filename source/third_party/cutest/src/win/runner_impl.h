#pragma once

#include "../logger.h"
#include "../runner_base.h"

#include <set>
#include <wtypes.h>

CUTEST_NS_BEGIN

class RunnerImpl
    : public RunnerBase {
public:
    RunnerImpl();
    virtual ~RunnerImpl();

protected:
    Logger m_testProgressLogger;

public:
    // Runner的接口实现
    virtual void AsyncRunOnMainThread(Runnable* runnable, bool isAutoDelete);
    virtual void DelayRunOnMainThread(unsigned int msDelay, Runnable* runnable, bool isAutoDelete);

    virtual void WaitUntilAllTestEnd();

protected:
    enum {
        WM_RUN = WM_USER + 1,
        WM_DELAY_RUN_AUTO_DELETE,
        WM_DELAY_RUN,
    };
    static HWND s_hMessageWnd;
    static LRESULT CALLBACK MessageWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

    static VOID CALLBACK DelayRunAndDelete(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse);
    static VOID CALLBACK DelayRun(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse);

    static std::set<Runnable*> s_autoDeleteRunnables;  // 用于缓存DelayRunOnMainThread()中尚未执行且需要自动Delete的Runnable对象
};

CUTEST_NS_END
