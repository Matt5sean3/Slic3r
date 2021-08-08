
#include <emscripten/bind.h>
// Allow setting up a print via
#include "SimplePrint.hpp"

using namespace emscripten;

// Very much a crazy global concern
const PrintConfigDef & get_print_config_def( )
{
  return print_config_def;
}

EMSCRIPTEN_BINDINGS(slic3r_bindings) {

  register_vector< std::string >( "StringVector" );

  enum_< ConfigOptionType >( "ConfigOptionType" )
    .value( "coNone", coNone )
    .value( "coFloat", coFloat )
    .value( "coFloats", coFloats )
    .value( "coInt", coInt )
    .value( "coInts", coInts )
    .value( "coString", coString )
    .value( "coStrings", coStrings )
    .value( "coPercent", coPercent )
    .value( "coFloatOrPercent", coFloatOrPercent )
    .value( "coPoint", coPoint )
    .value( "coPoints", coPoints )
    .value( "coPoint3", coPoint3 )
    .value( "coBool", coBool )
    .value( "coBools", coBools )
    .value( "coEnum", coEnum );

  class_< ConfigOptionDef >( "ConfigOptionDef" )
    .property( "type", &ConfigOptionDef::type )
    ;

  // A vital class for knowing what settings there are
  class_<PrintConfigDef>( "PrintConfigDef" )
    ;

  class_<ConfigOption>( "ConfigOption" )
    .function( "set", &ConfigOption::set )
    .property( "bool", &ConfigOption::getBool, &ConfigOption::setBool )
    .property( "int", &ConfigOption::getInt, &ConfigOption::setInt )
    .property( "float", &ConfigOption::getFloat, &ConfigOption::setFloat )
    .property( "string", &ConfigOption::getString, &ConfigOption::setString )
    .property( "getStrings", &ConfigOption::getStrings, &ConfigOption::setStrings )
    ;

  // Print configuration is probably the biggest issue
  // DynamicPrintConfig : PrintConfig, DynamicConfig
  // Multiple inheritance is a slight problem, but from the Javascript side we care about DynamicConfig
  class_<DynamicPrintConfig>( "DynamicPrintConfig" )
    .constructor<>( )
    // .property( "def", &ConfigBase::def )
    .function( "normalize", &DynamicPrintConfig::normalize )
    .function( "set_deserialize", &PrintConfigBase::set_deserialize )
    .function( "min_object_distance", &PrintConfigBase::min_object_distance )
    .function( "has", &ConfigBase::has )
    .function( "save", &ConfigBase::save )
    .function( "load", &ConfigBase::load )
    .function( "validate", &ConfigBase::validate )
    .function( "get_option", &DynamicConfig::optptr, allow_raw_pointers( ) )
    .function( "keys", &DynamicConfig::keys )
    .function( "erase", &DynamicConfig::erase )
    .function( "clear", &DynamicConfig::clear )
    .function( "empty", &DynamicConfig::empty )
    // Allow command line configuration
    .function( "read_cli",
        select_overload< void(
          const std::vector<std::string>&,
          t_config_option_keys*,
          t_config_option_keys*)>(
            &DynamicConfig::read_cli ),
        allow_raw_pointers( ) )
    .function( "read_cli",
        select_overload< bool(
          int,
          char**,
          t_config_option_keys*,
          t_config_option_keys* ) >(
            &DynamicConfig::read_cli ),
        allow_raw_pointers( ) )
    ;

  // ModelObject has a private destructor, which presents some issues for binding
  /*
  class_<ModelObject>( "ModelObject" )
    ;
    */
  class_<ModelMaterialMap>( "ModelMaterialMap" )
    ;

  class_<ModelMaterial>( "ModelMaterial" )
    .property( "config", &ModelMaterial::config )
    ;

  class_<BoundingBoxf3>( "BoundingBoxf3" )
    ;

  class_<Pointf>( "Pointf" )
    ;

  class_<Pointfs>( "Pointfs" )
    ;

  class_<BoundingBoxf>( "BoundingBoxf" )
    ;

  class_<TriangleMesh>( "TriangleMesh" )
    ;

  class_<Model>( "Model" )
    .constructor<>()
    .property( "materials", &Model::materials )
    .property( "objects", &Model::objects )
    .property( "metadata", &Model::metadata )
    .class_function( "read_from_file", &Model::read_from_file )
    .function( "merge", &Model::merge )
    .function( "create_object", select_overload<ModelObject*( )>( &Model::add_object ), allow_raw_pointers( ) )
    .function( "add_object", select_overload<ModelObject*( const ModelObject &, bool )>( &Model::add_object ), allow_raw_pointers( ) )
    .function( "delete_object", &Model::delete_object ) 
    .function( "clear_objects", &Model::clear_objects )
    .function( "create_material", select_overload<ModelMaterial*( t_model_material_id ) >( &Model::add_material ), allow_raw_pointers( ) )
    .function( "add_material", select_overload<ModelMaterial*( t_model_material_id, const ModelMaterial & ) >( &Model::add_material ), allow_raw_pointers( ) )
    .function( "get_material", &Model::get_material, allow_raw_pointers( ) )
    .function( "delete_material", &Model::delete_material )
    .function( "clear_materials", &Model::clear_materials )
    .function( "has_objects_with_no_instances", &Model::has_objects_with_no_instances )
    .function( "add_default_instances", &Model::add_default_instances )
    .function( "bounding_box", &Model::bounding_box )
    .function( "repair", &Model::repair )
    .function( "split", &Model::split )
    .function( "center_instances_around_point", &Model::center_instances_around_point )
    .function( "align_instances_to_origin", &Model::align_instances_to_origin )
    .function( "align_to_ground", &Model::align_to_ground )
    .function( "translate", &Model::translate )
    .function( "mesh", &Model::mesh )
    .function( "raw_mesh", &Model::raw_mesh )
    .function( "arrange", &Model::_arrange, allow_raw_pointers( ) )
    .function( "arrange_objects", &Model::arrange_objects, allow_raw_pointers( ) )
    .function( "duplicate", &Model::duplicate, allow_raw_pointers( ) )
    .function( "duplicate_objects", &Model::duplicate_objects, allow_raw_pointers( ) )
    .function( "duplicate_objects_grid", &Model::duplicate_objects_grid )
    // .function( "print_info", &Model::print_info )
    .function( "looks_like_multipart_object", &Model::looks_like_multipart_object )
    .function( "convert_multipart_object", &Model::convert_multipart_object )
    ;

  class_<Print>( "Print" )
    ;

  class_<SimplePrint>( "SimplePrint" )
    .constructor<>()
    .property( "arrange", &SimplePrint::arrange )
    .property( "center", &SimplePrint::center )
    .property( "status_cb", &SimplePrint::status_cb )
    .function( "apply_config", &SimplePrint::apply_config )
    .function( "total_used_filament", &SimplePrint::total_used_filament )
    .function( "total_extruded_volume", &SimplePrint::total_extruded_volume )
    .function( "export_gcode", &SimplePrint::export_gcode )
    .property( "model", &SimplePrint::model, &SimplePrint::set_model )
    .property( "print", &SimplePrint::print )
    ;

}

