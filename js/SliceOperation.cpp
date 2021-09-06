
#include "interface.h"
#include "SliceOperation.hpp"
#include "BrowserThreadQueue.hpp"
#include <ThreadPool.hpp>
#include <PrintConfig.hpp>
#include <SimplePrint.hpp>
#include <sstream>

typedef std::pair< std::string, std::string > NameUrlPair;

typedef std::vector< NameUrlPair > NameUrlPairs;

extern "C" void EMSCRIPTEN_KEEPALIVE do_callback( std::function< void( ) > * callback_ptr )
{
  auto callback = *callback_ptr;
  callback( );
  delete callback_ptr;
}

static void enqueue_show_status( std::string text )
{
  // show_status needs to happen on the browser thread
  BrowserThreadQueue::global( ).enqueue( [=] ( ) {
        show_status( text.c_str( ) );
      } );
}

static void enqueue_provide_gcode( std::string gcode_filename, std::string gcode_url )
{
  BrowserThreadQueue::global( ).enqueue( [=] ( ) {
        provide_gcode( gcode_filename.c_str( ), gcode_url.c_str( ) );
      } );
}

static void status_callback( int progress, const std::string & message )
{
  std::ostringstream oss;
  oss << "Status: " << progress << "% " << message << std::endl;
  enqueue_show_status( oss.str( ) );
}

// === slice browser callable function ===
extern "C" void EMSCRIPTEN_KEEPALIVE slice(
    const char * settings_file_name_ptr ,
    const char * settings_file_url_ptr ,
    const char * model_file_name_ptr ,
    const char * model_file_url_ptr  )
{
  using namespace Slic3r;

  // Needed for memory management on asynchronous lambda functions
  std::string settings_file_name( settings_file_name_ptr );
  std::string settings_file_url( settings_file_url_ptr );
  std::string model_file_name( model_file_name_ptr );
  std::string model_file_url( model_file_url_ptr );

  // show_status needs to be on the 
  enqueue_show_status( "Sending slice command." );
  auto func = std::function< void( ) >( [=] ( ) {

    FullPrintConfig full_print_config;
    DynamicPrintConfig print_config;
    // Adapted from CLI code
    ConfigDef config_def;
    config_def.merge( cli_actions_config_def );
    config_def.merge( cli_transform_config_def );
    config_def.merge( cli_misc_config_def );
    config_def.merge( print_config_def );

    DynamicConfig config;
    config.def = &config_def;

    DynamicPrintConfig file_config;
    file_config.load( std::string( "/slicer_files/" ) + settings_file_name );
    file_config.normalize( );
    print_config.apply( file_config );
    print_config.apply( config, true );
    print_config.normalize( );

    full_print_config.apply( print_config, true );

    auto valid = full_print_config.validate_noexcept( );
    if( ! valid.first )
    {
      std::ostringstream oss;
      oss << "Full print config validation failure: " << valid.second << std::endl;
      enqueue_show_status( oss.str( ).c_str( ) );
      return;
    }
    enqueue_show_status( "Loaded configuration" );
    // Create the model
    auto model = Model::read_from_file( std::string( "/slicer_files/" ) + model_file_name );
    model.add_default_instances( );

    enqueue_show_status( "Loaded model" );

    SimplePrint print;
    print.apply_config( print_config );
    print.set_model( model );
    print.status_cb = &status_callback;

    std::string gcode_file_path( std::string( "/" ) + model_file_name + ".gcode" );
    enqueue_show_status( "Slicing..." );

    auto result = print.export_gcode_noexcept( gcode_file_path );
    if( result.first )
    {
      enqueue_provide_gcode( gcode_file_path, generate_url( gcode_file_path.c_str( ) ) );
    }
    else
    {
      enqueue_show_status( result.second );
    }
  });
  
  ThreadPool::global( ).enqueue( [=] ( ) {
        // Build a tuple to send to javascript in a buffer
        mount_urls(
            "/slicer_files",
            NameUrlPairs{
              NameUrlPair( settings_file_name, settings_file_url ),
              NameUrlPair( model_file_name, model_file_url ) },
            func );
      } );
}

