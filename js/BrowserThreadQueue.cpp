
#include "BrowserThreadQueue.hpp"
// emscripten_performance_now
#include <emscripten/html5.h>

std::unique_ptr< BrowserThreadQueue > BrowserThreadQueue::singleton;

BrowserThreadQueue::BrowserThreadQueue( )
{
}

BrowserThreadQueue::~BrowserThreadQueue( )
{
}

BrowserThreadQueue &
BrowserThreadQueue::global( )
{
  if( ! singleton )
  {
    singleton.reset( new BrowserThreadQueue );
  }
  return *singleton;
}

void
BrowserThreadQueue::process( )
{
  long long int start_time = emscripten_performance_now( );

  do
  {
    std::function< void( ) > action;
    {
      // minimize time the mutex is locked
      std::unique_lock queue_lock( queue_mutex );
      if( action_queue.empty( ) )
      {
        break;
      }
      action = action_queue.front( );
      action_queue.pop( );
    }
    action( );
    // Place a time limit on the frame of 50 ms
    // That is a little time, but should not hurt responsiveness
  }
  while( emscripten_performance_now( ) - start_time > 50.0 );
}

void
BrowserThreadQueue::enqueue( std::function< void( ) > task )
{
  std::unique_lock queue_lock( queue_mutex );
  action_queue.push( task );
}

