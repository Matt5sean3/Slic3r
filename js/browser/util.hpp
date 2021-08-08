
#ifndef browser_util_h_
#define browser_util_h_

#include "../common/Structured.h"
#include <emscripten/emscripten.h>

extern worker_handle worker;

template< class ResponseProtocol >
void handle_response(
    char * response,
    int response_size,
    void * userdata )
{
  ResponseProtocol * protocol = static_cast< ResponseProtocol * >( userdata );
  protocol->decode( response + sizeof( bool ) );
  if( ! Structured::Field< bool >::decode( response ) )
  { // Received a non-provisional response, clean up the userdata
    delete protocol;
  }
}


template< class RequestProtocol, typename RequestProtocol::MessageSelector Selector, class ResponseProtocol, class... RequestMessageArgs >
void send_command( const ResponseProtocol & responseProtocol, RequestMessageArgs... args )
{
  ResponseProtocol * responseProtocolCopy = new ResponseProtocol( responseProtocol );
  std::vector< char > buffer;
  RequestProtocol::template encode< Selector >(
      buffer,
      typename RequestProtocol::template SelectAncestor< Selector >::type::Message( args... ) );
  // Managing memory with this API is a real challenge
  emscripten_call_worker(
      worker,
      "do_command",
      buffer.data( ),
      buffer.size( ),
      handle_response< ResponseProtocol >,
      responseProtocolCopy );
}

#endif
