#pragma once

#include <cppunit/portability/CppUnitMap.h>
#include <cppunit/portability/CppUnitSet.h>
#include <cppunit/portability/CppUnitVector.h>

#include <map>

/*
  1. TestFactoryRegistry内部有一个std::set<TestFactory*>类型的m_factories；
  2. m_factories用于保存TestFactoryRegistry::registerFactory(TestFactory*)的入参；
  3. TestFactoryRegistry::addTestToSuite(TestSuite*)中会遍历m_factories，逐个元素调用TestSuite::addTest(Test*)；
  4. 由于std::set的无序性，registerFactory()和addTest()的顺序会不一致，这个会影响到用例的执行顺序；
  5. TestFactorySort可以用来保证registerFactory()和addTest()的顺序一致；
  6. Windows上使用TestFactorySort来让界面上的用例树更好看；
  7. 每个测试用例都应该是独立的，互不影响的，相互依赖的用例不是好用例。
*/
CPPUNIT_NS_BEGIN

class TestFactory;

class TestFactorySort
{
public:
  static TestFactorySort *getInstance();

  // 分配排序的权值，内部会缓存入参的值用于QueryWeigth()
  unsigned int allocWeigth( const TestFactory *factory );

  // 查找之前AllocWeigth()分配的权值
  unsigned int queryWeigth( const TestFactory *factory );

  // 移除分配的权值，不移除的话，内部缓存的会越来越大
  void deallocWeigth( const TestFactory *factory );

  // 得到按权值排序的数组
  typedef std::less<TestFactory *> TestFactoryLess;
  void sort( const CppUnitSet<TestFactory *, TestFactoryLess> &factories, CppUnitVector<TestFactory *> &result );

protected:
  TestFactorySort();

  unsigned int current_weigth;
  CppUnitMap<const TestFactory *, unsigned int> factories_weigth;
};

CPPUNIT_NS_END
