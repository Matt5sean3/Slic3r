#ifndef STRUCTURED_H_
#define STRUCTURED_H_

// Flattening things into a working API isn't as important now that I know the threads API is in good shape
// Don't really want to do this if I don't need to.

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <type_traits>

#include "layout.hpp"

namespace Structured
{
  // The magic for structured really only needs to address parts that can't fit into a standard layout
  // Things that are "trivially copyable" just need layout information transmitted to javascript

  // The trick is then to handle data that cannot fit that paradigm, such as vectors and strings
  // However, these can just be treated as arrays of "trivially copyable" data without great difficulty.

  // The trick is then using a special allocator to handle other types
  template< class StaticType, class... DynamicTypes >
  struct DynamicData
  {
    static_assert( std::is_standard_layout_v< StaticType > );

    std::vector< char > buffer;
    std::tuple< DynamicTypes... > dynamic;
    StaticType & flat;

    DynamicData( ) :
      buffer( sizeof( StaticType ) ),
      dynamic( ),
      flat( *reinterpret_cast< StaticType * >( buffer.data( ) ) )
    {
    }

    const char * data( )
    {
    }

    void format_dynamic( )
    {
    }

  };

  // === Field ===
  // For very basic types
  template< class Type >
  struct Field
  {
    // Is the size known at compile time?
    static constexpr bool has_static_size = true;
    static constexpr size_t static_size = sizeof( Type );
  
    typedef Type FieldType;

    static size_t size( const char * buffer )
    {
      return sizeof( Type );
    }
  
    static Type decode( const char * buffer )
    {
      return *reinterpret_cast< const Type * >( buffer );
    }
  
    static void encode( std::vector< char > & buffer, const Type & field )
    {
      const char * field_data = reinterpret_cast< const char * >( &field );
      buffer.insert( buffer.end( ), field_data, field_data + sizeof( Type ) );
    }
  };

  template< class T >
  void encode( std::vector< char > & buffer, const T & field )
  {
    Field< T >::encode( buffer, field );
  }
  
  // Specialization for strings
  template<
      class charT,
      class traits,
      class Alloc >
  struct Field< std::basic_string< charT, traits, Alloc > >
  {
    static constexpr bool has_static_size = false;

    typedef std::basic_string< charT, traits, Alloc > FieldType;
  
    static size_t size( const char * buffer )
    {
      return *reinterpret_cast< const size_t * >( buffer );
    }
  
    static FieldType decode( const char * buffer )
    {
      std::vector< charT > vect;
      for(
          const char * cursor = buffer + sizeof( size_t );
          cursor < buffer + size( buffer );
          cursor += Field< charT >::static_size )
      {
        vect.push_back( Field< charT >::decode( cursor ) );
      }
      auto ret = FieldType( vect.begin( ), vect.end( ) );
      return ret;
    }
  
    static void encode( std::vector< char > & buffer, const FieldType & field )
    {
      std::vector< char > tmp_buffer;
      for( auto & entry : field )
      {
        Field< charT >::encode( tmp_buffer, entry );
      }
      size_t size = tmp_buffer.size( ) + sizeof( size_t );
      char * size_data = reinterpret_cast< char * >( &size );
      buffer.insert( buffer.end( ), size_data, size_data + sizeof( size ) );
      buffer.insert( buffer.end( ), tmp_buffer.begin( ), tmp_buffer.end( ) );
    }
  };
  
  // specialization for vectors
  template<
      class Type,
      class Allocator /* = std::allocator< Type > */ >
  struct Field< std::vector< Type, Allocator > >
  {
    static constexpr bool has_static_size = false;

    typedef std::vector< Type, Allocator > FieldType;
    static size_t size( const char * buffer )
    {
      return *reinterpret_cast< const size_t * >( buffer );
    }
  
    static FieldType decode( const char * buffer )
    {
      FieldType vect;
      for(
          const char * cursor = buffer + sizeof( size_t );
          cursor < buffer + size( buffer );
          cursor += Field< Type >::size( cursor ) )
      {
        vect.push_back( Field< Type >::decode( cursor ) );
      }
      return vect;
    }
  
    static void encode( std::vector< char > & buffer, const FieldType & field )
    {
      std::vector< char > tmp_buffer;
      for( auto & entry : field )
      {
        Field< Type >::encode( tmp_buffer, entry );
      }
      size_t size = tmp_buffer.size( ) + sizeof( size_t );
      char * size_data = reinterpret_cast< char * >( &size );
      buffer.insert( buffer.end( ), size_data, size_data + sizeof( size ) );
      buffer.insert( buffer.end( ), tmp_buffer.begin( ), tmp_buffer.end( ) );
    }
  };

  template<
      class T1,
      class T2 >
  struct Field< std::pair< T1, T2 > >
  {
    static constexpr bool has_static_size = Field< std::tuple< T1, T2 > >::has_static_size;
    typedef std::pair< T1, T2 > FieldType;

    static size_t size( const char * buffer )
    {
      return *reinterpret_cast< const size_t * >( buffer );
    }
    static FieldType decode( const char * buffer )
    {
      const char * cursor = buffer + sizeof( size_t );
      return FieldType(
          Field< T1 >::decode( cursor ),
          Field< T2 >::decode( cursor + Field< T1 >::size( cursor ) ) );
    }
    static void encode( std::vector< char > & buffer, const FieldType & field  )
    {
      std::vector< char > tmp_buffer;
      Field< T1 >::encode( tmp_buffer, field.first );
      Field< T2 >::encode( tmp_buffer, field.second );
      size_t size = tmp_buffer.size( ) + sizeof( size );
      char * size_data = reinterpret_cast< char * >( &size );
      buffer.insert( buffer.end( ), size_data, size_data + sizeof( size ) );
      buffer.insert( buffer.end( ), tmp_buffer.begin( ), tmp_buffer.end( ) );
    }
  };
  
  template<
      class Key,
      class Value,
      class Compare /* = std::less< Key > */,
      class Allocator /* = std::allocator< std::pair< const Key, Value > > */ >
  struct Field< std::map< Key, Value, Compare, Allocator > >
  {
    static constexpr bool has_static_size = false;

    typedef std::map< Key, Value, Compare, Allocator > FieldType;
    static size_t size( const char * buffer )
    {
      return *reinterpret_cast< const size_t * >( buffer );
    }
  
    static FieldType decode( const char * buffer )
    {
      FieldType result;
      for(
          const char * cursor = buffer + sizeof( size_t );
          cursor < buffer + size( buffer );
          cursor += Field< Value >::size( cursor ) )
      {
        Key k = Field< Key >::decode( cursor );
        cursor += Field< Key >::size( cursor );
        Value v = Field< Value >::decode( cursor );
        result[ k ] = v;
      }
      return result;
    }
  
    static void encode( std::vector< char > & buffer, const FieldType & field )
    {
      std::vector< char > tmp_buffer;
      for( auto & entry : field )
      {
        Field< Key >::encode( tmp_buffer, entry.first );
        Field< Value >::encode( tmp_buffer, entry.second );
      }
      size_t size = tmp_buffer.size( ) + sizeof( size_t );
      char * size_data = reinterpret_cast< char * >( &size );
      buffer.insert( buffer.end( ), size_data, size_data + sizeof( size ) );
      buffer.insert( buffer.end( ), tmp_buffer.begin( ), tmp_buffer.end( ) );
    }
  };

  // === NodeAncestor ===
  template< class Node, int Generations >
  struct NodeAncestor
  {
    typedef typename NodeAncestor< typename Node::Parent, Generations - 1 >::type type;
  };

  template< class Node >
  struct NodeAncestor< Node, 0 >
  {
    typedef Node type;
  };


  // Static tuple layout is some complex meta-programming
/*
  template< class... Members >
  struct StaticTupleLayout
  {
    // Filter members into byte sizes, 1, 2, 4, 8
    // small size, close packed
    filter_by_static_size_t< 0, 1, Members... > packed;
    // 2-aligned
    filter_by_static_size_t< 1, 2, Members... > two_aligned;
    // 4-aligned
    filter_by_static_size_t< 2, 4, Members... > four_aligned;
    // 8-aligned
    filter_by_static_size_t< 4, 65535, Members... > eight_aligned;

    static void encode( std::vector< char > & buffer, const FieldType & field )
    {
      inner_encode< 0 >( buffer, field );
    }

  private:
    template< size_t Alignment, class TypeList >
    static void encode_aligned( )
    {
    }

    template< size_t I >
    static void inner_encode( std::vector< char > & buffer, const FieldType & field )
    {
      Field< decltype( std::get< I >( field ) ) >::encode( buffer, std::get< I >( field ) );
      inner_encode< I + 1 >( buffer, field );
    }

    template<>
    static void inner_encode< sizeof...( Members ) >( std::vector< char > & buffer, const FieldType & field )
    {
    }
  };

  // Dynamic layout needs to be handled at runtime
  template< class... Members >
  struct DynamicTupleLayout
  {
  };

  // Tuples can be static or dynamic
  template< class... Members >
  struct TupleFieldStaticSize :
    StaticTupleLayout< Members... >
  {
    static constexpr bool has_static_size = true;
  };

  template< class... Members >
  struct TupleFieldDynamicSize :
    // Filter out the static fields to layout separately
    apply_template_to_type_list_t<
      StaticTupleLayout,
      typename filter_by_has_static_size< Members... >::type
    >
  {
    static constexpr bool has_static_size = false;
  };
  */

  // === std::tuple ===
  template< class... Members >
  struct Field< std::tuple< Members... > >
  {
    typedef std::tuple< Members... > FieldType;

    static size_t size( const char * buffer )
    {
      auto base_size = Field< typename FieldType::Member >::size( buffer );
      return base_size + Field< typename FieldType::Parent >::size( buffer + base_size );
    }

    static FieldType decode( const char * buffer )
    {
      return FieldType(
        Field< typename FieldType::Member >::decode( buffer ),
        Field< typename FieldType::Parent >::decode( buffer + Field< typename FieldType::Member >::size( buffer ) ) );
    }

    static void encode( std::vector< char > & buffer, const FieldType & field )
    {
      inner_encode< 0 >( buffer, field );
    }

  private:
    template< size_t I >
    static void inner_encode( std::vector< char > & buffer, const FieldType & field )
    {
      Field< decltype( std::get< I >( field ) ) >::encode( buffer, std::get< I >( field ) );
      inner_encode< I + 1 >( buffer, field );
    }

    template<>
    static void inner_encode< sizeof...( Members ) >( std::vector< char > & buffer, const FieldType & field )
    {
    }
  };

  template<>
  struct Field< std::tuple<> >
  {
    typedef std::tuple< > FieldType;

    static size_t size( const char * buffer )
    {
      return 0;
    }

    static FieldType decode( const char * buffer )
    {
      return FieldType( );
    }

    static void encode( std::vector< char > & buffer, const FieldType & field )
    {
    }
  };

}

#endif
