
#include "../common/Structured.h"
#include <iostream>

using namespace Structured;

// test signed int
bool test_field_int( )
{
  std::vector< char > buffer;
  Field< int >::encode( buffer, -5 );
  auto success = Field< int >::decode( buffer.data( ) ) == -5;
  if( ! success )
  {
    std::cout << "Field<int> encode-decode failed" << std::endl;
  }
  return success;
}

// test unsigned int
bool test_field_unsigned_int( )
{
  std::vector< char > buffer;
  Field< unsigned int >::encode( buffer, 5 );
  auto success = Field< unsigned int >::decode( buffer.data( ) ) == 5;
  if( ! success )
  {
    std::cout << "Field<unsigned int> encode-decode failed" << std::endl;
  }
  return success;
}

// test bool
bool test_field_bool( )
{
  std::vector< char > buffer;
  Field< bool >::encode( buffer, true );
  auto success = Field< bool >::decode( buffer.data( ) );
  if( ! success )
  {
    std::cout << "Field<bool> encode-decode failed" << std::endl;
  }
  return success;
}

// test char
bool test_field_char( )
{
  std::vector< char > buffer;
  Field< char >::encode( buffer, 'x' );
  auto success = Field< char >::decode( buffer.data( ) ) == 'x';
  if( ! success )
  {
    std::cout << "Field<char> encode-decode failed" << std::endl;
  }
  return success;
}

bool test_field_vector_int( )
{
  std::vector< char > buffer;
  std::vector< int > source{ 1, 1, 2, 3, 5, 8, 13 };
  Field< std::vector< int > >::encode( buffer, source );
  auto recreate = Field< std::vector< int > >::decode( buffer.data( ) );
  auto success = recreate.size( ) == source.size( );
  for( int i = 0; i < source.size( ); ++i )
  {
    success &= recreate[ i ] == source[ i ];
  }
  if( ! success )
  {
    std::cout << "Field<vector<int>> encode-decode failed" << std::endl;
  }
  return success;
}

bool test_field_string( )
{
  std::vector< char > buffer;
  Field< std::string >::encode( buffer, "Hello, World!" );
  auto success = Field< std::string >::decode( buffer.data( ) ) == "Hello, World!";
  if( ! success )
  {
    std::cout << "Field<std::string> encode-decode failed" << std::endl;
  }
  return success;
}

bool test_field_pair_int_int( )
{
  std::vector< char > buffer;
  Field< std::pair< int, int > >::encode( buffer, std::pair< int, int >( 42, 24 ) );
  auto success = Field< std::pair< int, int > >::decode( buffer.data( ) ) == std::pair< int, int >( 42, 24 );
  if( ! success )
  {
    std::cout << "Field<std::pair<int,int>> encode-decode failed" << std::endl;
  }
  return success;
}

bool test_message_string_string( )
{
  std::vector< char > buffer;
  Message< std::string, std::string > source( "Hello", "World" );
  source.encode( buffer );
  auto duplicate = Message< std::string, std::string >::decode( buffer.data( ) );
  auto success =
    duplicate.get< 0 >( ) == "Hello" &&
    duplicate.get< 1 >( ) == "World";
  if( ! success )
  {
    std::cout << "Message<std::string, std::string> encode-decode failed" << std::endl;
  }
  return success;
}

bool test_message_string_string_use( )
{
  std::vector< char > buffer;
  Message< std::string, std::string > source( "Hello", "World" );
  source.encode( buffer );
  auto duplicate = Message< std::string, std::string >::decode( buffer.data( ) );
  // That's a little disappointing, it couldn't just figure it out
  auto success = duplicate.use< bool >( [] ( std::string a, std::string b ) {
      return ( a == "Hello" ) && ( b == "World" );
  } );
  if( ! success )
  {
    std::cout << "Message<std::string, std::string>::use failed" << std::endl;
  }
  return success;
}

bool test_message_string_string_int_use( )
{
  std::vector< char > buffer;
  Message< std::string, std::string, int > source( "Hello", "World", 5 );
  source.encode( buffer );
  auto duplicate = Message< std::string, std::string, int >::decode( buffer.data( ) );
  // That's a little disappointing, it couldn't just figure it out
  auto success = duplicate.use< bool >( [] ( std::string a, std::string b, int c ) {
      return ( a == "Hello" ) && ( b == "World" ) && c == 5;
  } );
  if( ! success )
  {
    std::cout << "Message<std::string, std::string, int>::use failed" << std::endl;
  }
  return success;
}

bool test_message_string_string_int_use_no_return( )
{
  std::vector< char > buffer;
  Message< std::string, std::string, int > source( "Hello", "World", 5 );
  source.encode( buffer );
  auto duplicate = Message< std::string, std::string, int >::decode( buffer.data( ) );
  // That's a little disappointing, it couldn't just figure it out
  bool success = false;
  duplicate.use< void >( [&] ( std::string a, std::string b, int c ) {
      success = ( a == "Hello" ) && ( b == "World" ) && c == 5;
  } );
  if( ! success )
  {
    std::cout << "Message<std::string, std::string, int>::use getting result by reference failed" << std::endl;
  }
  return success;
}

int main( int argc, char ** argv )
{
  // Just some quick tests
  return (
      test_field_int( ) &&
      test_field_unsigned_int( ) &&
      test_field_bool( ) &&
      test_field_char( ) &&
      test_field_vector_int( ) &&
      test_field_string( ) &&
      test_field_pair_int_int( ) &&
      test_message_string_string_use( ) &&
      test_message_string_string_int_use( ) &&
      test_message_string_string_int_use_no_return( )
     ) ? 0 : 1;
}

