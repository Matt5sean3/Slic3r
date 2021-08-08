
#include <emscripten.h>
#include <emscripten/html5.h>
#include <SimplePrint.hpp>
#include <PrintConfig.hpp>

#include "utils.h"
#include "commands.h"

#include <memory>
#include <string>
#include <vector>

#include <cstring>
#include "../common/api.h"

#include <sstream>
#include <fstream>

template< class ResponseProtocol, typename ResponseProtocol::MessageSelector Message, class... Args >
void respond( bool provisional, Args... args )
{
  std::vector< char > buffer;
  Structured::Field< bool >::encode( buffer, provisional );
  typename ResponseProtocol::template SelectAncestor< Message >::type::Message message( args... );
  ResponseProtocol::template encode< Message >( buffer, message );
  if( provisional )
  {
    emscripten_worker_respond_provisionally( buffer.data( ), buffer.size( ) );
  }
  else
  {
    emscripten_worker_respond( buffer.data( ), buffer.size( ) );
  }
}

static void status_callback( int status, const std::string & message )
{
  std::ostringstream oss;
  oss << "Error #" << status << " " << message;
  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true, oss.str( ) );
}

struct SlicingFiles
{
  std::vector< std::string > settings_files;
  std::vector< std::string > model_files;
};

void slice_files_ready( void * userdata )
{
  using namespace Slic3r;

  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true, "Files ready" );

  auto slicing_files_ptr = static_cast< SlicingFiles * >( userdata );
  auto slicing_files = * slicing_files_ptr;
  std::string settings_file_name = slicing_files.settings_files[ 0 ];
  std::string model_file_name = slicing_files.model_files[ 0 ];
  delete slicing_files_ptr;

  // Create the configuration
  DynamicPrintConfig config;
  config.load( std::string( "/slicer_files/" ) + settings_file_name );
  config.normalize( );

  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true, "Loaded configuration" );
  // Create a model for printing
  Model model;
  // The suffix matters
  model.read_from_file( "/slicer_files/" + model_file_name );
  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true, "Loaded model" );

  // Unmount the files
  unmount_directory( "/slicer_files" );

  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true, "Unmounted files" );

  // Create a simple print
  SimplePrint print;

  print.apply_config( config );
  print.set_model( model );
  // Generate and export the gcode
  std::string gcode_file_path( std::string( "/" ) + model_file_name + ".gcode" );
  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true, "Slicing..." );
  print.status_cb = &status_callback;
  auto result = print.export_gcode_noexcept( gcode_file_path );
  if( result.first )
  {
    respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::SUCCESS >( false,
        model_file_name + ".gcode",
        generate_url( gcode_file_path.c_str( ) ) );
  }
  else
  {
    respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::ERROR >( false, result.second );
  }

}

// Send in the settings and model, receive the resulting sliced model
void slice(
    std::vector< std::pair< std::string, std::string > > settings_file_pairs,
    std::vector< std::pair< std::string, std::string > > model_file_pairs )
{
  respond< ModSlicer::SliceResponseProtocol, ModSlicer::SliceResponseMessage::PROGRESS >( true,
      "Worker received slice command" );

  // These need to be more C-like because they go to JS
  auto slicing_files = new SlicingFiles;
  std::vector< URLFile > files;
  for( const auto & pair : settings_file_pairs )
  {
    files.emplace_back(
        pair.first,
        pair.second );
    slicing_files->settings_files.push_back( pair.first );
  }
  for( const auto & pair : model_file_pairs )
  {
    files.emplace_back(
        pair.first,
        pair.second );
    slicing_files->model_files.push_back( pair.first );
  }

  mount_urls_async( "/slicer_files", files.data( ), sizeof( URLFile ), files.size( ), slice_files_ready, slicing_files );
}

void EMSCRIPTEN_KEEPALIVE call_basic_callback( basic_callback callback, void * userdata )
{
  callback( userdata );
}

std::unique_ptr< ModSlicer::RequestProtocol > request_protocol_ptr;

void EMSCRIPTEN_KEEPALIVE do_command( char * data, int size )
{
  if( ! request_protocol_ptr )
  {
    request_protocol_ptr = std::make_unique< ModSlicer::RequestProtocol >( );

    request_protocol_ptr->handle< ModSlicer::RequestMessage::SLICE >( slice );

    request_protocol_ptr->handle< ModSlicer::RequestMessage::LIST_SETTINGS >( [] ( ) {
          // Generate a list of settings
          std::vector< std::string > setting_names;
          for( auto config_pair : print_config_def.options )
          {
            setting_names.push_back( config_pair.first );
          }
          respond< ModSlicer::ListSettingsResponseProtocol, ModSlicer::ListSettingsResponseMessage::SUCCESS >( false, setting_names );
        });

    request_protocol_ptr->handle< ModSlicer::RequestMessage::UNKNOWN >( [] ( const char * buffer ) {
          std::ostringstream oss;
          oss << "First size_t: " << Structured::Field< size_t >::decode( buffer ) << std::endl;
        } );
  }
  request_protocol_ptr->decode( data );
}

