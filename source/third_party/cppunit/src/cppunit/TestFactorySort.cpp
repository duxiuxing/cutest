#include "TestFactorySort.h"
#include <algorithm>

CPPUNIT_NS_BEGIN

TestFactorySort *
TestFactorySort::getInstance()
{
  static TestFactorySort test_factory_sort;
  return &test_factory_sort;
}

TestFactorySort::TestFactorySort() : current_weigth( 0 )
{}

unsigned int
TestFactorySort::allocWeigth( const TestFactory *factory )
{
  CppUnitMap<const TestFactory *, unsigned int>::iterator it = this->factories_weigth.find( factory );
  if ( it == this->factories_weigth.end() )
  {
    ++this->current_weigth;
    this->factories_weigth[factory] = this->current_weigth;
    return this->current_weigth;
  }
  else
  {
    return it->second;
  }
}

unsigned int
TestFactorySort::queryWeigth( const TestFactory *factory )
{
  CppUnitMap<const TestFactory *, unsigned int>::iterator it = this->factories_weigth.find( factory );
  if ( it == this->factories_weigth.end() )
  {
    return 0;
  }
  else
  {
    return it->second;
  }
}

void
TestFactorySort::deallocWeigth( const TestFactory *factory )
{
  this->factories_weigth.erase( factory );
}

bool
sortByWeigth( const TestFactory *factoryA, const TestFactory *factoryB )
{
  TestFactorySort *instance = TestFactorySort::getInstance();
  unsigned int weigthA = instance->queryWeigth( factoryA );
  unsigned int weigthB = instance->queryWeigth( factoryB );
  return weigthA < weigthB;
}

void
TestFactorySort::sort( const CppUnitSet<TestFactory *, TestFactoryLess> &factories, CppUnitVector<TestFactory *> &result )
{
  CppUnitSet<TestFactory *, TestFactoryLess>::const_iterator it = factories.begin();
  while ( it != factories.end() )
  {
    result.push_back( *it );
    ++it;
  }
  std::sort( result.begin(), result.end(), sortByWeigth );
}

CPPUNIT_NS_END
