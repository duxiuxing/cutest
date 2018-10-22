#include "runner_impl.h"

#include <set>
#include "gmock/gmock.h"

CUTEST_NS_BEGIN

void InitGoogleMock() {
    static bool s_initGoogleMock = true;
    if (s_initGoogleMock) {
        s_initGoogleMock = false;
        testing::InitGoogleMock(&__argc, __wargv);
    }
}

unsigned long long TickCount64() {
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

thread_id CurrentThreadId() {
    return ::GetCurrentThreadId();
}

Runner* Runner::Instance() {
    static RunnerImpl s_runnerImpl;
    return &s_runnerImpl;
}

HWND RunnerImpl::s_hMessageWnd = NULL;
std::set<Runnable*> RunnerImpl::s_autoDeleteRunnables;

RunnerImpl::RunnerImpl() {
    InitGoogleMock();
    m_listenerManager.Add(&m_testProgressLogger);

    // Register message window class.
    WNDCLASSEX wcx;
    ::ZeroMemory(&wcx, sizeof(WNDCLASSEX));
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = MessageWindowProc;
    wcx.hInstance = ::GetModuleHandle(NULL);
    wcx.lpszClassName = L"CUTEST_MAIN_TEST_RUNNER";
    ::RegisterClassEx(&wcx);

    // Create a message window.
    RunnerImpl::s_hMessageWnd = ::CreateWindow(
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
    AsyncRunOnMainThread(this, false);
}

RunnerImpl::~RunnerImpl() {
    if (RunnerImpl::s_hMessageWnd) {
        ::DestroyWindow(RunnerImpl::s_hMessageWnd);
        RunnerImpl::s_hMessageWnd = NULL;
    }

    std::set<Runnable*>::iterator it = RunnerImpl::s_autoDeleteRunnables.begin();
    while (it != RunnerImpl::s_autoDeleteRunnables.end()) {
        delete *it;
        ++it;
    }
}

void RunnerImpl::AsyncRunOnMainThread(Runnable* runnable, bool isAutoDelete) {
    ::PostMessage(RunnerImpl::s_hMessageWnd, WM_RUN, (WPARAM)runnable, isAutoDelete);
}

void RunnerImpl::DelayRunOnMainThread(unsigned int msDelay, Runnable* runnable, bool isAutoDelete) {
    if (isAutoDelete) {
        ::PostMessage(RunnerImpl::s_hMessageWnd, WM_DELAY_RUN_AUTO_DELETE, (WPARAM)runnable, msDelay);
    } else {
        ::PostMessage(RunnerImpl::s_hMessageWnd, WM_DELAY_RUN, (WPARAM)runnable, msDelay);
    }
}

LRESULT CALLBACK RunnerImpl::MessageWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    LRESULT ret = 0;
    switch (msg) {
    case WM_RUN: {
            Runnable* runnable = (Runnable*)wparam;
            runnable->Run();
            if (lparam) {
                delete runnable;
            }
        }
        break;
    case WM_DELAY_RUN_AUTO_DELETE: {
            RunnerImpl::s_autoDeleteRunnables.insert((Runnable*)wparam);
            ::SetTimer(RunnerImpl::s_hMessageWnd, (UINT_PTR)wparam, (UINT)lparam, &RunnerImpl::DelayRunAndDelete);
        }
        break;
    case WM_DELAY_RUN: {
            ::SetTimer(RunnerImpl::s_hMessageWnd, (UINT_PTR)wparam, (UINT)lparam, &RunnerImpl::DelayRun);
        }
        break;
    default: {
            ret = DefWindowProc(wnd, msg, wparam, lparam);
        }
        break;
    }
    return ret;
}

VOID CALLBACK RunnerImpl::DelayRunAndDelete(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse) {
    ::KillTimer(wnd, runnablePtr);

    Runnable* runnable = (Runnable*)runnablePtr;
    RunnerImpl::s_autoDeleteRunnables.erase(runnable);
    runnable->Run();
    delete runnable;
}

VOID CALLBACK RunnerImpl::DelayRun(HWND wnd, UINT msg, UINT_PTR runnablePtr, DWORD msElapse) {
    ::KillTimer(wnd, runnablePtr);

    Runnable* runnable = (Runnable*)runnablePtr;
    runnable->Run();
}

void RunnerImpl::WaitUntilAllTestEnd() {
    MSG msg = {0};
    while (STATE_NONE != m_state
           && ::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    RunnerBase::WaitUntilAllTestEnd();
}

CUTEST_NS_END
