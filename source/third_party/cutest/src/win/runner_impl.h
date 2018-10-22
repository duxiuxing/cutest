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
    Logger testProgressLogger;

public:
    // Runner的接口实现
    virtual void asyncRunOnMainThread(Runnable* runnable, bool isAutoDelete);
    virtual void delayRunOnMainThread(unsigned int msDelay, Runnable* runnable, bool isAutoDelete);

    virtual void waitUntilAllTestEnd();

protected:
    enum {
        WM_RUN = WM_USER + 1,
        WM_DELAY_RUN_AUTO_DELETE,
        WM_DELAY_RUN,
    };
    static HWND hMessageWnd;
    static LRESULT CALLBACK messageWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

    static VOID CALLBACK onTimer4DelayRunAutoDelete(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse);
    static VOID CALLBACK onTimer4DelayRun(HWND wnd, UINT uMsg, UINT_PTR runnablePtr, DWORD msElapse);

    static std::set<Runnable*> autoDeleteRunnables;  // 用于缓存delayRunOnMainThread()中尚未执行且需要auto delete的Runnable对象
};

CUTEST_NS_END
