
mergeInto( LibraryManager.library,
  {
    mount_urls_async: async function( directory_ptr, url_file_ptr, url_file_stride, num_url_files, on_completion, userdata_ptr )
    {
      let directory = UTF8ToString( directory_ptr );
      let names = [ ];
      let urls = [ ];
      for( let i = 0; i < num_url_files; ++i )
      {
        names.push( UTF8ToString( _URLFile_get_name( url_file_ptr + url_file_stride * i ) ) );
        urls.push( UTF8ToString( _URLFile_get_url( url_file_ptr + url_file_stride * i ) ) );
      }
      let files = ( await Promise.all(
        urls.map( url => fetch( url ).then( response => response.blob( ) ) ) ) )
      .map( ( blob, idx ) => ( { name: names[ idx ], data: blob } ) );
      
      FS.mkdir( directory );
      FS.mount( WORKERFS, {
        files: [ ],
        blobs: files
      }, directory );
      _call_basic_callback( on_completion, userdata_ptr );
    },
    unmount_directory: function( directory_ptr )
    {
      FS.unmount( UTF8ToString( directory_ptr ) );
    },
    generate_url: function( path_ptr )
    {
      let path = UTF8ToString( path_ptr );
      // Should be sufficient to make the file accessible via JS
      return URL.createObjectURL( new Blob( FS.readFile( path, { encoding: "binary" } ) ) );
    },
    destroy_url: function( path )
    {
      URL.revokeObjectURL( path );
    }
  });
