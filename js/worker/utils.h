#ifndef UTILS_H_
#define UTILS_H_

#include <string>

extern "C" {

// Mount a set of files provided as a URL
typedef void ( * basic_callback )( void * );

typedef struct URLFile URLFile;

struct URLFile
{
  URLFile(
      const std::string & file_name,
      const std::string & file_url );
  std::string name;
  std::string url;
};

const char * URLFile_get_name( struct URLFile * file );
const char * URLFile_get_url( struct URLFile * file );
URLFile * URLFile_get_next( struct URLFile * file );

void call_basic_callback( basic_callback callback, void * userdata );

extern void mount_urls_async(
    const char * directory,
    URLFile * files,
    size_t file_stride,
    size_t num_files,
    basic_callback on_completion,
    void * userdata );

extern void unmount_directory( const char * directory );

// Generate a URL from a file
// Note that the return value must be free'd explicitly
extern char * generate_url(
    const char * path );

extern void destroy_url(
    const char * url );

}

#endif
