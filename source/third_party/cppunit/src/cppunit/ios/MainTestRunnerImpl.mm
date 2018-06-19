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
    return MainTestRunnerImpl::main_thread_id;
}

MainTestRunner *Runner::instance() {
    static MainTestRunnerImpl main_test_runner;
    return &main_test_runner;
}

thread_id MainTestRunnerImpl::main_thread_id = 0;

MainTestRunnerImpl::MainTestRunnerImpl() : run_loop(NULL) {
    this->run_loop = [MainTestRunLoop new];
    _listenerManager.add(&test_progress_logger);

    // 通过这个异步方法给main_thread_id赋值
    asyncRunOnMainThread(this, false);
}

MainTestRunnerImpl::~MainTestRunnerImpl() {}

void MainTestRunnerImpl::asyncRunOnMainThread(Runnable *runnable,
                                              bool is_auto_delete) {
    [this->run_loop asyncRunOnMainThread:runnable isAutoDelete:is_auto_delete];
}

void MainTestRunnerImpl::delayRunOnMainThread(unsigned int delay_ms,
                                              Runnable *runnable,
                                              bool is_auto_delete) {
    [this->run_loop delayRunOnMainThread:delay_ms
                       runnable:runnable
                   isAutoDelete:is_auto_delete];
}

void MainTestRunnerImpl::run() {
    MainTestRunnerImpl::main_thread_id = Runner::currentThreadId();
}

CPPUNIT_NS_END
