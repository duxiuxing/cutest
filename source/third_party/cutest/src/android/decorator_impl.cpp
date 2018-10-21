#include "decorator_impl.h"
#include "SynchronizationObjectImpl.h"

CUTEST_NS_BEGIN

Decorator*
Decorator::createInstance(CPPUNIT_NS::Test* test) {
    return new DecoratorImpl(test);
}

DecoratorImpl::DecoratorImpl(CPPUNIT_NS::Test* test)
    : TestDecorator(test)
    , test_result(new CPPUNIT_NS::SynchronizationObjectImpl(), new CPPUNIT_NS::SynchronizationObjectImpl())
    , result_collector(new CPPUNIT_NS::SynchronizationObjectImpl())
    , runing_test(NULL) {
    test_result.addListener(this);
    test_result.addListener(&this->result_collector);

    this->run_completed = Event::createInstance();
}

DecoratorImpl::~DecoratorImpl() {
    this->run_completed->wait();
    this->run_completed->destroy();
    CPPUNIT_NS::TestDecorator::m_test = NULL;
}

void
DecoratorImpl::destroy() {
    delete this;
}

void
DecoratorImpl::addListener(CPPUNIT_NS::TestListener* listener) {
    this->test_result.addListener(listener);
}

void
DecoratorImpl::start() {
    this->run_completed->reset();

    pthread_attr_t attr;
    ::pthread_attr_init(&attr);

    sched_param sched;
    ::pthread_attr_getschedparam(&attr, &sched);
    sched.sched_priority = ::sched_get_priority_max(0);
    ::pthread_attr_setschedparam(&attr, &sched);

    pthread_t thread;
    ::pthread_create(&thread, &attr, threadFunction, this);
}

void*
DecoratorImpl::threadFunction(void* param) {
    DecoratorImpl* decorator = (DecoratorImpl*)param;

    decorator->runOnWorkerThread();

    return param;
}

void
DecoratorImpl::runOnWorkerThread() {
    this->test_result.runTest(this);

    this->run_completed->post();
}

void
DecoratorImpl::stop() {
    this->test_result.stop();
}

const CPPUNIT_NS::TestResultCollector*
DecoratorImpl::testResultCollector() {
    return &this->result_collector;
}

void
DecoratorImpl::startTest(CPPUNIT_NS::Test* test) {
    this->runing_test = test;
}

void
DecoratorImpl::endTest(CPPUNIT_NS::Test* test) {
    this->runing_test = NULL;
}

void
DecoratorImpl::addFailure(bool isError, CPPUNIT_NS::Exception* exception) {
    if (isError) {
        this->test_result.addError(this->runing_test, exception);
    } else {
        this->test_result.addFailure(this->runing_test, exception);
    }
}

CUTEST_NS_END
