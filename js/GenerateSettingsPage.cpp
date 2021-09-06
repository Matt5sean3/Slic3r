
// The main thread still needs to do the actual page construction
// for responsiveness, placing a time limit on this is important
#include "BrowserThreadQueue.hpp"
#include <PrintConfig.hpp>
#include <ConfigBase.hpp>
#include "interface.h"

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

// A static global
static std::unique_ptr< std::map< std::string, std::vector< ConfigDefTuple > > > settings_groups;

// === MAIN THREAD TASKS ===
// Displays a settings group on the page
void display_group( const char * group_name )
{
  if( settings_groups )
  {
    auto group_iter = settings_groups->find( group_name );
    if( group_iter != settings_groups->end( ) )
    {
      for( auto & setting : group_iter->second )
      {
        display_setting( setting );
      }
    }
  }
}

// === THREADABLE ===

typedef std::map< std::string, std::vector< Slic3r::ConfigOptionDef * > > sorted_configuration;


void initialize_settings( )
{
  // Sort the configuration into groups
  settings_groups = std::make_unique< std::map< std::string, std::vector< ConfigDefTuple > > >( );
  for( auto & config_option : Slic3r::print_config_def.options )
  {
    auto & config_def = config_option.second;
    auto emplaced = settings_groups->emplace( std::make_pair( config_def.category, std::vector< ConfigDefTuple >( ) ) );
    emplaced.first->second.emplace_back(
       config_def.type,
       config_def.label,
       config_def.full_label,
       config_def.tooltip,
       config_def.sidetext,
       config_def.multiline,
       config_def.full_width,
       config_def.height,
       config_def.width,
       config_def.min,
       config_def.max,
       config_def.shortcut,
       config_def.enum_values,
       config_def.enum_labels );
  }
}

