
#include "interface.h"
#include "util.hpp"
#include <emscripten/emscripten.h>
#include "ListSettingsOperation.hpp"
#include "../common/api.h"
#include <sstream>

#include <emscripten/html5.h>

void EMSCRIPTEN_KEEPALIVE list_settings( )
{
  ModSlicer::ListSettingsResponseProtocol * list_settings_response = new ModSlicer::ListSettingsResponseProtocol;
  list_settings_response->handle< ModSlicer::ListSettingsResponseMessage::ERROR >(
      [] ( auto error_message ) { show_status( error_message.c_str( ) ); } );
  list_settings_response->handle< ModSlicer::ListSettingsResponseMessage::SUCCESS >(
      [] ( auto settings ) {
        emscripten_console_log( "Received list of settings" );
        std::ostringstream oss;
        for( auto setting : settings )
        {
          oss << setting << ", ";
        }
        show_status( oss.str( ).c_str( ) );
      } );
  send_command< ModSlicer::RequestProtocol, ModSlicer::RequestMessage::LIST_SETTINGS >( *list_settings_response );
}

