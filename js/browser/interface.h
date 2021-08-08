#ifndef SEND_COMMAND_H_
#define SEND_COMMAND_H_

#include <emscripten.h>

extern "C" {

extern void initialize_interface( );
extern void show_status( const char * status_text );
extern void provide_gcode( const char * filename, const char * url );

void stop_slicer( worker_handle slicer_handle );

}

#endif
