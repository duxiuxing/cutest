#include "MainTestRunnerImpl.h"

#include <cppunit/Runnable.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <mach/mach_init.h>
#include <mach/mach_time.h>
#include <stdio.h>

@implementation MainTestRunLoop

- (void)asyncRunOnMainThread:(CPPUNIT_NS::Runnable *)runnable
                isAutoDelete:(bool)is_auto_delete {
    NSValue *arg = [NSValue valueWithPointer:runnable];

    if (is_auto_delete) {
        [self performSelectorOnMainThread:@selector(runAndDelete:)
                               withObject:arg
                            waitUntilDone:NO];
    } else {
        [self performSelectorOnMainThread:@selector(runOnly:)
                               withObject:arg
                            waitUntilDone:NO];
    }
}

- (void)delayRunOnMainThread:(unsigned int)delay_ms
                    runnable:(CPPUNIT_NS::Runnable *)runnable
                isAutoDelete:(bool)is_auto_delete {
    dispatch_time_t delayTime =
        dispatch_time(DISPATCH_TIME_NOW, delay_ms * NSEC_PER_MSEC);

    dispatch_after(delayTime, dispatch_get_main_queue(), ^{
      [self asyncRunOnMainThread:runnable isAutoDelete:is_auto_delete];
    });
}

- (void)runOnly:(NSValue *)arg {
    CPPUNIT_NS::Runnable *runnable =
        (CPPUNIT_NS::Runnable *)([arg pointerValue]);
    runnable->run();
}

- (void)runAndDelete:(NSValue *)arg {
    CPPUNIT_NS::Runnable *runnable =
        (CPPUNIT_NS::Runnable *)([arg pointerValue]);
    runnable->run();
    delete runnable;
}

@end

CPPUNIT_NS_BEGIN

unsigned long long Runner::tickCount() {
    static mach_timebase_info_data_t info = {0};
    static kern_return_t krv __attribute__((unused)) = mach_timebase_info(&info);
    static double r = 1.0 * info.numer / (info.denom * NSEC_PER_MSEC);
    return (unsigned long long)(mach_absolute_time() * r);
}

thread_id Runner::currentThreadId() {
    return mach_thread_self();
}

thread_id Runner::mainThreadId() {
    return MainTestRunnerImpl::s_mainThreadId;
}

MainTestRunner *Runner::instance() {
    static MainTestRunnerImpl s_mainTestRunner;
    return &s_mainTestRunner;
}

thread_id MainTestRunnerImpl::s_mainThreadId = 0;

MainTestRunnerImpl::MainTestRunnerImpl() : _runLoop(NULL) {
    _runLoop = [MainTestRunLoop new];
    _listenerManager.add(&_testProgressLogger);

    // 通过这个异步方法给s_mainThreadId赋值
    asyncRunOnMainThread(this, false);
}

MainTestRunnerImpl::~MainTestRunnerImpl() {}

void MainTestRunnerImpl::asyncRunOnMainThread(Runnable *runnable,
                                              bool is_auto_delete) {
    [_runLoop asyncRunOnMainThread:runnable isAutoDelete:is_auto_delete];
}

void MainTestRunnerImpl::delayRunOnMainThread(unsigned int delay_ms,
                                              Runnable *runnable,
                                              bool is_auto_delete) {
    [_runLoop delayRunOnMainThread:delay_ms
                       runnable:runnable
                   isAutoDelete:is_auto_delete];
}

void MainTestRunnerImpl::run() {
    MainTestRunnerImpl::s_mainThreadId = Runner::currentThreadId();
}

CPPUNIT_NS_END
