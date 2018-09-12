#include "MainTestDecoratorImpl.h"
#include "SynchronizationObjectImpl.h"

CPPUNIT_NS_BEGIN

MainTestDecorator* MainTestDecorator::createInstance(Test* test) {
    return new MainTestDecoratorImpl(test);
}

MainTestDecoratorImpl::MainTestDecoratorImpl(Test* test)
    : TestDecorator(test)
    , _testResult(new SynchronizationObjectImpl())
    , _resultCollector(new SynchronizationObjectImpl())
    , _state(STATE_NONE)
    , _testRuning(NULL) {
    _testResult.addListener(this);
    _testResult.addListener(&_resultCollector);

    _runCompleted = TestEvent::createInstance();
}

MainTestDecoratorImpl::~MainTestDecoratorImpl() {
    _runCompleted->wait();
    _runCompleted->destroy();
    m_test = NULL;
}

void MainTestDecoratorImpl::destroy() {
    delete this;
}

void MainTestDecoratorImpl::addListener(TestListener* listener) {
    _testResult.addListener(listener);
}

void MainTestDecoratorImpl::start() {
    if (STATE_RUNING == _state)
        return;

    _state = STATE_RUNING;

    _runCompleted->reset();

    pthread_attr_t attr;
    ::pthread_attr_init(&attr);

    sched_param sched;
    ::pthread_attr_getschedparam(&attr, &sched);
    sched.sched_priority = ::sched_get_priority_max(0);
    ::pthread_attr_setschedparam(&attr, &sched);

    pthread_t thread;
    ::pthread_create(&thread, &attr, threadFunction, this);
}

void* MainTestDecoratorImpl::threadFunction(void* pThis) {
    MainTestDecoratorImpl* decorator = (MainTestDecoratorImpl*)pThis;

    decorator->runOnWorkerThread();

    return pThis;
}

void MainTestDecoratorImpl::runOnWorkerThread() {
    if (STATE_RUNING == _state)
        _testResult.runTest(this);

    _runCompleted->post();
}

void MainTestDecoratorImpl::stop() {
    if (STATE_NONE == _state)
        return;

    _state = STATE_NONE;
    _testResult.stop();
}

const TestResultCollector* MainTestDecoratorImpl::testResultCollector() {
    return &_resultCollector;
}

void MainTestDecoratorImpl::startTest(Test* test) {
    _testRuning = test;
}

void MainTestDecoratorImpl::endTest(Test* test) {
    _testRuning = NULL;
}

void MainTestDecoratorImpl::addFailure(bool is_error, Exception* exception) {
    if (is_error)
        _testResult.addError(_testRuning, exception);
    else
        _testResult.addFailure(_testRuning, exception);
}

CPPUNIT_NS_END