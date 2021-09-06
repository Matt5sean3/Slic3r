#ifndef slic3r_ThreadPool_hpp_
#define slic3r_ThreadPool_hpp_

#include <thread>
#include <queue>
#include <condition_variable>
#include <emscripten/emscripten.h>

namespace Slic3r {

// Rather than spin up threads to handle parallelism, just have threads at the ready
class ThreadPool
{
public:
  typedef std::function< void( ) > Task;
  typedef std::weak_ptr< Task > TaskHandle;
  typedef std::shared_ptr< Task > TaskPtr;
private:
  static std::unique_ptr< ThreadPool > global_thread_pool;

  std::mutex queue_mutex;

  std::condition_variable task_added;
  std::condition_variable task_complete;

  // All threads needs to wait
  // This is necessary to allow safely exfiltrating data to Javascript

  // The pauser locks the queue_mutex and the pause_mutex disallowing all other threads from continuing
  int pause_count;
  std::mutex pause_mutex;
  std::condition_variable pause_complete;

  std::queue< TaskPtr > task_queue;
  std::vector< std::thread > pool;

  bool running;

  void service_function( );
  ThreadPool( );
public:

  virtual ~ThreadPool( );
  static ThreadPool & global( );

  TaskHandle enqueue( Task task );

  // Wait on a range of tasks
  template< class Range >
  void wait( Range begin, Range end )
  {
    // Operates using contents of the queue
    std::unique_lock< std::mutex > queue_lock( queue_mutex );
    for( ; begin != end; ++begin )
    {
      while( ! begin->expired( ) )
      {
        // Wait until we're notified that a task completed
        task_complete.wait( queue_lock );
      }
    }
  }

};

}

#endif
