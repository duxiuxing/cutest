#pragma once

#include "gtest/internal/gtest-port.h"

class CountDownLatchImpl;

class GTEST_API_ CountDownLatch
{
public:
  // count表示countDown()需要被调用count次才会结束等待
  CountDownLatch( int count );
  virtual ~CountDownLatch();

  /*
    调用此方法时：
    - 如果m_count为0，则直接返回；
    - 如果m_count比0大，会一直等待，直到m_count变成0
  */
  void await();

  /*
    timeout_ms表示最长等待时间，单位为毫秒。
    调用此方法时：
    - 如果m_count为0，则直接返回true；
    - 如果m_count比0大，会等待一段时间，等待时间内如果m_count变成0，返回true；
    - 如果超出等待时间，m_count还没有变成0，返回false；
  */
  bool await( unsigned int timeout_ms );

  // 内部会对m_count减1
  void countDown();

  // 返回当前m_count的值
  int getCount();

protected:
  CountDownLatchImpl *m_impl;
};
