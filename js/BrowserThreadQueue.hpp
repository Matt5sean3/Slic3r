#ifndef Slicerjs_BrowserThreadQueue_h_
#define Slicerjs_BrowserThreadQueue_h_

#include <functional>
#include <queue>
#include <mutex>

class BrowserThreadQueue
{
private:
  BrowserThreadQueue( );
  std::mutex queue_mutex;
  std::queue< std::function< void( ) > > action_queue;

  static std::unique_ptr< BrowserThreadQueue > singleton;
public:
  virtual ~BrowserThreadQueue( );
  static BrowserThreadQueue & global( );
  static void main_loop( );

  void enqueue( std::function< void( ) > action );
  void process( );
};

#endif
