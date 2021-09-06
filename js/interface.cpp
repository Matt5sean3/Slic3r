
#include "interface.h"
#include <cstdint>

// Some have wrapper functions to isolate memory management issues
extern "C" {

extern void JS_initialize_interface( );
extern void JS_show_status( const char * status_text );
extern void JS_provide_gcode( const char * filename, const char * url );
extern void JS_display_setting( const char * config_def_tuple_ptr );
extern void JS_mount_urls(
    const char * url_pair_array_ptr,
    size_t url_pair_array_size,
    std::function< void() > * func_ptr );
extern char * JS_generate_url( const char * file_path );
extern void JS_destroy_url( const char * file_path );
void C_do_callback( std::function< void( ) > * func_ptr );

}

void initialize_interface( )
{
  JS_initialize_interface( );
}

void show_status( std::string status_text )
{
  JS_show_status( status_text.c_str( ) );
}

void provide_gcode( std::string name, std::string url )
{
  JS_provide_gcode( name.c_str( ), url.c_str( ) );
}

void display_setting( ConfigDefTuple config_def_tuple )
{
  std::vector< char > buffer;
  Structured::encode( buffer, config_def_tuple );
  JS_display_setting( buffer.data( ) );
}

void mount_urls(
    std::string mount_point,
    std::vector< std::pair< std::string, std::string > > url_pairs,
    std::function< void( ) > func )
{
  // Strings get aligned to 2-byte borders
  size_t buffer_size = 2 + mount_point.size( );
  for( auto url_pair : url_pairs )
  {
    buffer_size += 4;
    buffer_size += url_pair.first.size( );
    buffer_size += url_pair.second.size( );
  }
  std::vector< char > mount_data_buffer( buffer_size );
  auto size_ptr = reinterpret_cast< uint16_t * >( mount_data_buffer.data( ) );
  * size_ptr = mount_point.size( );
  for( auto url_pair : url_pairs )
  {
    size_ptr = reinterpret_cast< uint16_t * >( mount_data_buffer.data( ) );
    buffer_size += 4;
    buffer_size += url_pair.first.size( );
    buffer_size += url_pair.second.size( );
  }
  // calculate the required size
  JS_mount_urls(
      mount_data_buffer.data( ),
      mount_data_buffer.size( ),
      new std::function< void( ) >( func ));
}

std::string generate_url( std::string path )
{
  char * url_ptr = JS_generate_url( path.c_str( ) );
  std::string url( url_ptr );
  free( url_ptr );
  return url;
}

void destroy_url( std::string path )
{
  JS_destroy_url( path.c_str( ) ); 
}

extern "C" void C_do_callback( std::function< void( ) > * func_ptr )
{
  ( *func_ptr )( );
  delete func_ptr;
}

