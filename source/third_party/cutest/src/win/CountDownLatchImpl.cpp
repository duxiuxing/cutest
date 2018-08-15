#pragma once

#include "../CountDownLatchImpl.h"

#include "cutest/Helper.h"
#include "cutest/Runner.h"

#include <Windows.h>

CountDownLatchImpl::CountDownLatchImpl( int count_in )
  : count( count_in )
  , event( NULL )
{}

CountDownLatchImpl::~CountDownLatchImpl()
{
  if ( this->event )
  {
    this->event->destroy();
    this->event = NULL;
  }
}

void
CountDownLatchImpl::await()
{
  if ( CUTEST_NS::isOnMainThread() )
  {
    awaitOnMainThread();
  }
  else
  {
    awaitOnWorkerThread();
  }
}

bool
CountDownLatchImpl::await( unsigned int timeout_ms )
{
  if ( CUTEST_NS::isOnMainThread() )
  {
    return awaitOnMainThread( timeout_ms );
  }
  else
  {
    return awaitOnWorkerThread( timeout_ms );
  }
}

void
CountDownLatchImpl::countDown()
{
  if ( InterlockedDecrement( reinterpret_cast<volatile LONG *>( &this->count ) ) <= 0
       && this->event )
  {
    this->event->post();
  }
}

int
CountDownLatchImpl::getCount()
{
  return this->count;
}

void
CountDownLatchImpl::awaitOnMainThread()
{
  MSG msg = {0};
  while ( this->count > 0
          && ::GetMessage( &msg, NULL, 0, 0 ) )
  {
    ::TranslateMessage( &msg );
    ::DispatchMessage( &msg );
  }
}

bool
CountDownLatchImpl::awaitOnMainThread( unsigned int timeout_ms )
{
  unsigned long long start = CUTEST_NS::tickCount64();

  MSG msg = {0};
  while ( this->count > 0
          && ::GetMessage( &msg, NULL, 0, 0 ) )
  {
    ::TranslateMessage( &msg );
    ::DispatchMessage( &msg );

    if ( ( CUTEST_NS::tickCount64() - start ) >= timeout_ms )
    {
      return false;
    }
  }

  return true;
}

void
CountDownLatchImpl::awaitOnWorkerThread()
{
  this->event = CUTEST_NS::Event::createInstance();
  this->event->wait();
}

bool
CountDownLatchImpl::awaitOnWorkerThread( unsigned int timeout_ms )
{
  this->event = CUTEST_NS::Event::createInstance();
  this->event->wait( timeout_ms );
  return ( this->count <= 0 );
}
