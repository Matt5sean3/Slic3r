#ifndef Layout_hpp_
#define Layout_hpp_

#include <cstring>
#include "meta/type_list.hpp"
#include "meta/type_list_size.hpp"
#include "meta/filter_by_has_static_size.hpp"
#include "meta/filter_by_has_dynamic_size.hpp"
#include "meta/filter_by_static_size.hpp"

namespace Layout
{
  // A format readable in Javascript
  struct FieldLayoutInfo
  {
    char field_name[ 32 ];
    size_t type;
    size_t offset;
    FieldLayoutInfo(
        size_t type_value,
        size_t offset_value,
        const char * name ) :
      type( type_value ),
      offset( offset_value )
    {
      strncpy( field_name, name, sizeof( field_name ) - 1 );
      field_name[ sizeof( field_name ) - 1 ] = 0;
    }

  };

  template< class FieldList >
  struct ClassLayoutInfo
  {
    // Should really just use alignment type traits
    typedef Meta::filter_by_has_static_size_t< FieldList > StaticFieldList;
    typedef Meta::filter_by_static_size_t< 0, 1, StaticFieldList > OneByteFieldList;
    typedef Meta::filter_by_static_size_t< 1, 2, StaticFieldList > TwoByteFieldList;
    typedef Meta::filter_by_static_size_t< 2, 4, StaticFieldList > FourByteFieldList;
    typedef Meta::filter_by_static_size_t< 4, 65535, StaticFieldList > EightByteFieldList;
    typedef Meta::filter_by_has_dynamic_size_t< FieldList > DynamicFieldList;

    size_t size_one_byte_fields;
    size_t size_two_byte_fields;
    size_t size_four_byte_fields;
    size_t size_eight_byte_fields;
    size_t size_dynamic_fields;
  
    FieldLayoutInfo one_byte_fields[ Meta::type_list_size_v< OneByteFieldList > ];
    FieldLayoutInfo two_byte_fields[ Meta::type_list_size_v< TwoByteFieldList > ];
    FieldLayoutInfo four_byte_fields[ Meta::type_list_size_v< FourByteFieldList > ];
    FieldLayoutInfo eight_byte_fields[ Meta::type_list_size_v< EightByteFieldList > ];
    FieldLayoutInfo dynamic_fields[ Meta::type_list_size_v< DynamicFieldList > ];
  
    ClassLayoutInfo( ) :
      size_one_byte_fields( Meta::type_list_size_v< OneByteFieldList > ),
      size_two_byte_fields( Meta::type_list_size_v< TwoByteFieldList > ),
      size_four_byte_fields( Meta::type_list_size_v< FourByteFieldList > ),
      size_eight_byte_fields( Meta::type_list_size_v< EightByteFieldList > ),
      size_dynamic_fields( Meta::type_list_size_v< DynamicFieldList > )
    {
    }
  };
  
  struct LayoutMeta
  {
    unsigned char size_size_t;
    unsigned char size_unsigned_int;
  
    LayoutMeta( ) :
      size_size_t( sizeof( size_t ) ),
      size_unsigned_int( sizeof( unsigned int ) )
    {
    }

  };
  
  static_assert( std::is_standard_layout_v< LayoutMeta > );
  static_assert( std::is_standard_layout_v< ClassLayoutInfo< Meta::type_list< > > > );
  static_assert( std::is_standard_layout_v< FieldLayoutInfo > );

  // Javascript accessible functions
  extern "C" {
    // Needs to be implemented in Javascript
    extern void layout_initialize_js(
        LayoutMeta * meta );
  }
}

#endif
