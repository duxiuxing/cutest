#pragma once

#import <Foundation/Foundation.h>

#include "../MainTestRunnerBase.h"
#include "../Logger.h"

@interface MainTestRunLoop : NSObject

- (void)asyncRunOnMainThread:(CPPUNIT_NS::Runnable *)runnable
                isAutoDelete:(bool)is_auto_delete;

- (void)delayRunOnMainThread:(unsigned int)delay_ms
                    runnable:(CPPUNIT_NS::Runnable *)runnable
                isAutoDelete:(bool)is_auto_delete;

- (void)runOnly:(NSValue *)arg;

- (void)runAndDelete:(NSValue *)arg;

@end

CPPUNIT_NS_BEGIN

class MainTestRunnerImpl
    : public CUTEST_NS::RunnerBase
    , public Runnable {
    friend class MainTestRunner;
public:
    MainTestRunnerImpl();
    virtual ~MainTestRunnerImpl();

protected:
    TestProgressLogger test_progress_logger;

public:
    // MainTestRunner的接口实现
    virtual void asyncRunOnMainThread(Runnable *runnable,
                                      bool is_auto_delete);
    virtual void delayRunOnMainThread(unsigned int delay_ms, Runnable *runnable,
                                      bool is_auto_delete);

protected:
    MainTestRunLoop *run_loop;
    static thread_id main_thread_id;

    // 实现Runnable::Run()
    virtual void Run();
};

CPPUNIT_NS_END