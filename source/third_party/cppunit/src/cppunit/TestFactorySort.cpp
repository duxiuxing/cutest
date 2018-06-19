#include "TestFactorySort.h"
#include <algorithm>

CPPUNIT_NS_BEGIN

TestFactorySort *
TestFactorySort::getInstance()
{
  static TestFactorySort test_factory_sort;
  return &test_factory_sort;
}

TestFactorySort::TestFactorySort() : m_weigth( 0 )
{}

unsigned int
TestFactorySort::allocWeigth( const TestFactory *factory )
{
  CppUnitMap<const TestFactory *, unsigned int>::iterator it = m_weigthCache.find( factory );
  if ( it == m_weigthCache.end() )
  {
    ++m_weigth;
    m_weigthCache[factory] = m_weigth;
    return m_weigth;
  }
  else
  {
    return it->second;
  }
}

unsigned int
TestFactorySort::queryWeigth( const TestFactory *factory )
{
  CppUnitMap<const TestFactory *, unsigned int>::iterator it = m_weigthCache.find( factory );
  if ( it == m_weigthCache.end() )
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
  m_weigthCache.erase( factory );
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
