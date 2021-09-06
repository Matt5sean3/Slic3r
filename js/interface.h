#ifndef SEND_COMMAND_H_
#define SEND_COMMAND_H_

#include <emscripten.h>
#include <string>
#include <vector>
#include <functional>

// a settings definition Tuple is formatted as
typedef std::tuple<
  int, // type
  std::string, // label
  std::string, // full_label
  std::string, // tooltip
  std::string, // sidetext
  bool, // multiline
  bool, // full_width
  int, // height
  int, // width
  int, // min
  int, // max
  std::vector< std::string >, // shortcut
  std::vector< std::string >, // enum_values
  std::vector< std::string >  // enum_labels
> ConfigDefTuple;


void initialize_interface( );
void show_status( std::string status_text );
void provide_gcode( std::string filename, std::string url );
void display_setting( ConfigDefTuple config_def_tuple );
void mount_urls( std::string mount_point, std::vector< std::pair< std::string, std::string > > url_pairs, std::function< void() > func );
std::string generate_url( std::string file_path );


#endif
