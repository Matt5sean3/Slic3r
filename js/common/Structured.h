#ifndef STRUCTURED_H_
#define STRUCTURED_H_

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Structured
{
  // The goal is to create a method for encoding data at compile-time, but this
  // all really needs to interface with JS too. If I limit encoding to a small
  // set of possibilities I could probably allow exporting the format such that
  // JS could dynamically build matching structures.

  // === Field ===
  // For very basic types
  template< class Type >
  struct Field
  {
    typedef Type FieldType;

    // Is the size known at compile time?
    static constexpr bool has_static_size = true;
    static constexpr size_t static_size = sizeof( Type );
  
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
  
  // Specialization for strings
  template<
      class charT,
      class traits,
      class Alloc >
  struct Field< std::basic_string< charT, traits, Alloc > >
  {
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
  
  // === Message ===
  template< class... FieldTypes >
  struct Message
  {
    template< class R >
    struct HandlerType
    {
      typedef std::function< R( ) > type;
    };

    Message( )
    {
    }
  
    static Message decode( const char * buffer )
    {
      return Message( );
    }
  
    static size_t size( const char * buffer )
    {
      return 0;
    }
  
    void encode( std::vector< char > & buffer ) const
    {
    }

    template< class R >
    R use( std::function< R( ) > cb )
    {
      cb( );
    }

  protected:
    template< class R, class... D >
    R inner_use( std::function< R( D... ) > cb, D... args )
    {
      return cb( args... );
    }
  };
  

  template< class FieldType, class... FieldTypes >
  struct Message< FieldType, FieldTypes... > :
    public Message< FieldTypes... >
  {
  public:
    typedef FieldType ContainedType;
    typedef Message< FieldType, FieldTypes... > Self;
    typedef Message< FieldTypes... > Parent;

    template< int Generations >
    struct Ancestor
    {
      typedef typename NodeAncestor< Self, Generations >::type type;
    };

    // copy constructor
    Message( const Message & original ) :
      Parent( original ),
      field_( original.field_ )
    {
    }
  
    // from-fields constructor
    Message( FieldType field, FieldTypes... fields ) :
      Parent( fields... ),
      field_( field )
    {
    }
  
  private:
    Message( FieldType field, Parent base ) :
      Parent( base ),
      field_( field )
    {
    }

  protected:
    FieldType field_;
  
  public:
    static Message decode( const char * buffer )
    {
      return Message(
          Field< FieldType >::decode( buffer ),
          Parent::decode( buffer + Field< FieldType >::size( buffer ) ) );
    }
  
    static size_t size( const char * buffer )
    {
      size_t field_size = Field< FieldType >::size( buffer );
      return field_size + Parent::size( buffer + field_size );
    }
  
    void encode( std::vector< char > & buffer ) const
    {
      Field< FieldType >::encode( buffer, field_ );
      Parent::encode( buffer );
    }

    template< int Generations >
    typename Ancestor< Generations >::type::ContainedType
    get( )
    {
      return Ancestor< Generations >::type::field_;
    }

    template< class R >
    struct HandlerType
    {
      typedef std::function< R( FieldType, FieldTypes... ) > type;
    };

    template< class R >
    R use( typename HandlerType< R >::type cb )
    {
      return inner_use< R >( cb );
    }
  protected:
    template< class R, class... D >
    R inner_use( std::function< R( D..., FieldType, FieldTypes... ) > cb, D... args )
    {
      return Parent::template inner_use< R, D..., decltype( field_ ) >( cb, args..., field_ );
    }
  };

  template< class... MessageTypes >
  class Protocol
  {
  public:
    typedef std::function< void( const char * ) > HandlerType;
  protected:
    constexpr static size_t message_type_id = 0;
    std::vector< std::function< void( const char * ) > > decoders;
    HandlerType handler;

    void decode( const char * buffer )
    {
      // Call a default handler
      handler( buffer );
    }

    static void encode( )
    {
    }
  };

  template< class MessageType, class... MessageTypes >
  class Protocol< MessageType, MessageTypes... > :
    public Protocol< MessageTypes... >
  {
  public:
    typedef Protocol< MessageType, MessageTypes... > Self;
    typedef Protocol< MessageTypes... > Parent;
    typedef MessageType Message;
    typedef typename MessageType::template HandlerType< void >::type HandlerType;

    template< int Generations >
    struct Ancestor
    {
      typedef typename NodeAncestor< Self, Generations >::type type;
    };

    constexpr static size_t message_type_id = Protocol< MessageTypes... >::message_type_id + 1;
  protected:
    HandlerType handler;
  public:

    template< int Generations >
    static void encode(
        std::vector< char > & buffer,
        const typename Ancestor< Generations >::type::Message & message )
    {
      Field< size_t >::encode(
          buffer,
          Ancestor< Generations >::type::message_type_id );
      message.encode( buffer );
    }

    void decode( const char * buffer )
    {

      auto id = Field< size_t >::decode( buffer );
      if( id == message_type_id )
      {
        MessageType::decode(
              buffer +
              Field< size_t >::size( buffer ) ).template use< void >( handler );
      }
      else
      {
        Parent::decode( buffer );
      }
    }

    template< int Generations >
    void handle( typename Ancestor< Generations >::type::HandlerType newHandler )
    {
      Ancestor< Generations >::type::handler = newHandler;
    }

  };

  template< class BaseProtocol, class Enumeration >
  class NamedMessageProtocol :
    public BaseProtocol
  {
  private:
    BaseProtocol bp_;
  public:
    typedef Enumeration MessageSelector;

    template< Enumeration Message >
    struct SelectAncestor
    {
      typedef typename BaseProtocol::template Ancestor< static_cast< size_t >( Message ) >::type type;
    };

    template< Enumeration Message >
    static void encode(
        std::vector< char > & buffer,
        const typename SelectAncestor< Message >::type::Message & message )
    {
      BaseProtocol::template encode< static_cast< size_t >( Message ) >( buffer, message );
    }

    template< Enumeration Message >
    void handle( typename BaseProtocol::template Ancestor< static_cast< size_t>( Message ) >::type::HandlerType newHandler )
    {
      BaseProtocol::template handle< static_cast< size_t >( Message ) >( newHandler );
    }
  };

}

#endif
