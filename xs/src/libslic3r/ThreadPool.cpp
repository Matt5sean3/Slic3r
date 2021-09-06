#include "ThreadPool.hpp"
#include <emscripten/html5.h>

namespace Slic3r
{

std::unique_ptr< ThreadPool > ThreadPool::global_thread_pool;

void
ThreadPool::service_function( )
{
  // Start servicing the queue
  while( running )
  {
    TaskPtr task_ptr;
    {
      std::unique_lock< std::mutex > pause_lock( pause_mutex );
      ++pause_count;
    }
    {
      std::unique_lock< std::mutex > queue_lock( queue_mutex );
      if( task_queue.empty( ) )
      { // Wait here until a new task is added
        task_added.wait( queue_lock );
      }

      task_ptr = task_queue.front( );
      task_queue.pop( );
    }
    // Do the task, let then let the pointer expire
    ( *task_ptr )( );
  }
}

ThreadPool::ThreadPool( ) :
  pause_count( 0 ),
  running( true )
{ // Start the threads up
  for( int i = 0; i < 4; ++i )
  {
    pool.emplace_back( &ThreadPool::service_function, this );
  }
}

ThreadPool::~ThreadPool( )
{
}

ThreadPool &
ThreadPool::global( )
{
  if( ! global_thread_pool )
  {
    global_thread_pool = std::unique_ptr< ThreadPool >( new ThreadPool );
  }
  return *global_thread_pool;
}

ThreadPool::TaskHandle
ThreadPool::enqueue( Task task )
{
  std::unique_lock< std::mutex >( queue_lock );
  task_queue.emplace( new Task( task ) );
  return task_queue.back( );
}

}

