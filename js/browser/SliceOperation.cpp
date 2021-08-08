
#include "util.hpp"
#include "interface.h"
#include "SliceOperation.hpp"
#include "../common/api.h"

// === slice browser callable function ===
extern "C" void EMSCRIPTEN_KEEPALIVE slice(
    const char * settings_file_name,
    const char * settings_file_url,
    const char * model_file_name,
    const char * model_file_url )
{
  ModSlicer::SliceResponseProtocol * slice_response = new ModSlicer::SliceResponseProtocol;

  slice_response->handle< ModSlicer::SliceResponseMessage::ERROR >(
      [] ( auto error_message ) { show_status( error_message.c_str( ) ); } );

  slice_response->handle< ModSlicer::SliceResponseMessage::PROGRESS >(
      [] ( auto progress_message ) { show_status( progress_message.c_str( ) ); } );

  slice_response->handle< ModSlicer::SliceResponseMessage::SUCCESS >(
      [] ( std::string gcode_file_name, std::string gcode_file_url ) {
        // Final response
        show_status( "Slicing complete" );
        // Populate a link to download the finished gcode
        provide_gcode(
            gcode_file_name.c_str( ),
            gcode_file_url.c_str( ) );
      } );

  send_command< ModSlicer::RequestProtocol, ModSlicer::RequestMessage::SLICE >(
      *slice_response,
      std::vector< std::pair< std::string, std::string > >{ { settings_file_name, settings_file_url } },
      std::vector< std::pair< std::string, std::string > >{ { model_file_name, model_file_url } });
}

