#include "runner_impl.h"

#include <set>
#include "gmock/gmock.h"

CUTEST_NS_BEGIN

void
initGoogleMock() {
    static bool init_once = true;
    if (init_once) {
        init_once = false;
        testing::InitGoogleMock(&__argc, __wargv);
    }
}

unsigned long long
tickCount64() {
    static LARGE_INTEGER ticksPerSecond = {0};
    LARGE_INTEGER tick;
    if (!ticksPerSecond.QuadPart) {
        ::QueryPerformanceFrequency(&ticksPerSecond);
    }
    ::QueryPerformanceCounter(&tick);
    LONGLONG seconds = tick.QuadPart / ticksPerSecond.QuadPart;
    LONGLONG leftPart = tick.QuadPart - (ticksPerSecond.QuadPart * seconds);
    LONGLONG millSeconds = leftPart * 1000 / ticksPerSecond.QuadPart;
    return (unsigned long long)seconds * 1000 + millSeconds;
}

thread_id
currentThreadId() {
    return ::GetCurrentThreadId();
}

Runner*
Runner::instance() {
    static RunnerImpl runnerImpl;
    return &runnerImpl;
}

HWND RunnerImpl::hMessageWnd = NULL;
std::set<Runnable*> RunnerImpl::autoDeleteRunnables;

RunnerImpl::RunnerImpl() {
    initGoogleMock();
    this->listenerManager.add(&this->testProgressLogger);

    // Register message window class.
    WNDCLASSEX wcx;
    ::ZeroMemory(&wcx, sizeof(WNDCLASSEX));
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = messageWindowProc;
    wcx.hInstance = ::GetModuleHandle(NULL);
    wcx.lpszClassName = L"CUTEST_MAIN_TEST_RUNNER";
    ::RegisterClassEx(&wcx);

    // Create a message window.
    RunnerImpl::hMessageWnd = ::CreateWindow(
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
                                  NULL);              // no window-creation data

    // 通过这个异步方法给main_thread_id赋值
    asyncRunOnMainThread(this, false);
}

RunnerImpl::~RunnerImpl() {
    if (RunnerImpl::hMessageWnd) {
        ::DestroyWindow(RunnerImpl::hMessageWnd);
        RunnerImpl::hMessageWnd = NULL;
    }

    std::set<Runnable*>::iterator it = RunnerImpl::autoDeleteRunnables.begin();
    while (it != RunnerImpl::autoDeleteRunnables.end()) {
        delete *it;
        ++it;
    }
}

void
RunnerImpl::asyncRunOnMainThread(Runnable* runnable, bool isAutoDelete) {
    ::PostMessage(RunnerImpl::hMessageWnd, WM_RUN, (WPARAM)runnable, isAutoDelete);
}

void
RunnerImpl::delayRunOnMainThread(unsigned int msDelay, Runnable* runnable, bool isAutoDelete) {
    if (isAutoDelete) {
        ::PostMessage(RunnerImpl::hMessageWnd, WM_DELAY_RUN_AUTO_DELETE, (WPARAM)runnable, msDelay);
    } else {
        ::PostMessage(RunnerImpl::hMessageWnd, WM_DELAY_RUN, (WPARAM)runnable, msDelay);
    }
}

LRESULT CALLBACK
RunnerImpl::messageWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    LRESULT ret = 0;
    switch (msg) {
    case WM_RUN: {
            Runnable* runnable = (Runnable*)wparam;
            runnable->run();
            if (lparam) {
                delete runnable;
            }
        }
        break;
    case WM_DELAY_RUN_AUTO_DELETE: {
            RunnerImpl::autoDeleteRunnables.insert((Runnable*)wparam);
            ::SetTimer(RunnerImpl::hMessageWnd, (UINT_PTR)wparam, (UINT)lparam, &RunnerImpl::onTimer4DelayRunAutoDelete);
        }
        break;
    case WM_DELAY_RUN: {
            ::SetTimer(RunnerImpl::hMessageWnd, (UINT_PTR)wparam, (UINT)lparam, &RunnerImpl::onTimer4DelayRun);
        }
        break;
    default: {
            ret = DefWindowProc(wnd, msg, wparam, lparam);
        }
        break;
    }
    return ret;
}

VOID CALLBACK
RunnerImpl::onTimer4DelayRunAutoDelete(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse) {
    ::KillTimer(wnd, runnablePtr);

    Runnable* runnable = (Runnable*)runnablePtr;
    RunnerImpl::autoDeleteRunnables.erase(runnable);
    runnable->run();
    delete runnable;
}

VOID CALLBACK
RunnerImpl::onTimer4DelayRun(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse) {
    ::KillTimer(wnd, runnablePtr);

    Runnable* runnable = (Runnable*)runnablePtr;
    runnable->run();
}

void
RunnerImpl::waitUntilAllTestEnd() {
    MSG msg = {0};
    while (STATE_NONE != this->state
           && ::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    RunnerBase::waitUntilAllTestEnd();
}

CUTEST_NS_END
