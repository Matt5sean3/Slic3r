
#include <vector>
#include <string>
#include <ThreadPool.hpp>

#include "interface.h"
#include <emscripten/html5.h>

// Server is global
worker_handle worker;

int main( int argc, char ** argv )
{
  worker = emscripten_create_worker( "slicerjs_worker.js" );
  initialize_interface( );
  // Start the thread pool and enqueue a dummy task
  Slic3r::ThreadPool::global( ).enqueue( [ ] ( ) { emscripten_console_log( "Processed in a separate thread" ); } );

  return 0;
}

