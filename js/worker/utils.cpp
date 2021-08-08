
#include "utils.h"
#include <emscripten.h>

URLFile::URLFile(
    const std::string & file_name,
    const std::string & file_url ) :
  name( file_name ),
  url( file_url )
{
}

const char * EMSCRIPTEN_KEEPALIVE URLFile_get_name( URLFile * file )
{
  return file->name.c_str( );
}

const char * EMSCRIPTEN_KEEPALIVE URLFile_get_url( URLFile * file )
{
  return file->url.c_str( );
}

