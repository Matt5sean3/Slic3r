
#include "interface.h"
#include "ListSettingsOperation.hpp"

#include<vector>
#include<string>

#include "../common/api.h"
#include "ListSettingsOperation.hpp"

// Server is global
worker_handle worker;

int main( int argc, char ** argv )
{
  worker = emscripten_create_worker( "slicerjs_worker.js" );
  initialize_interface( );
  return 0;
}

