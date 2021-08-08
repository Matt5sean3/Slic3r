
// Establish
mergeInto( LibraryManager.library, {

  initialize_interface: function( )
  {

    let slice_button = document.getElementById( 'slice_button' );
    let settings_files_chooser = document.getElementById( 'settings_files_chooser' );
    let model_files_chooser = document.getElementById( 'model_files_chooser' );
    let list_settings_button = document.getElementById( 'list_settings_button' );

    list_settings_button.addEventListener( 'click', function( ) {
      console.log( "Listing Settings!" );
      _list_settings( );
    } );

    slice_button.addEventListener( 'click', function( ) {
      let settings_files = settings_files_chooser.files;
      let model_files = model_files_chooser.files;
      if( settings_files.length < 1 || model_files.length < 1 )
        return;
      let settings_file = settings_files[0];
      let settings_url = URL.createObjectURL( settings_file );
      let model_file = model_files[0];
      // Allocate cstrings
      let allocate_UTF8_string = function( str )
      {
        let str_length = lengthBytesUTF8( str ) + 1;
        let str_ptr = _malloc( str_length );
        stringToUTF8( str, str_ptr, str_length );
        return str_ptr;
      }

      args = [ settings_file.name,
        URL.createObjectURL( settings_file ),
        model_file.name,
        URL.createObjectURL( model_file ) ]
      .map( allocate_UTF8_string );

      _slice.apply( null, args );

      args.forEach( _free );
    } );

    var update_disable_slice_button = function( )
    {
      slice_button.disabled = settings_files_chooser.files.length < 1 || model_files_chooser.files.length < 1;
    };

    settings_files_chooser.addEventListener( 'change', update_disable_slice_button );
    model_files_chooser.addEventListener( 'change', update_disable_slice_button );
    update_disable_slice_button( );
  },
  show_status: function( status_text_ptr )
  {
    let status_text = UTF8ToString( status_text_ptr );
    let status_element = document.getElementById( 'status' );
    if( status_element.childNodes.length == 1 && status_element.childNodes.item( 0 ) instanceof Text )
    {
      status_element.childNodes[0].data = status_text;
    }
    else
    {
      status_element.childNodes.forEach( status_element.removeChild.bind( status_element ) );
      status_element.appendChild( document.createTextNode( status_text ) );
    }
  },
  provide_gcode: function( filename, url )
  {
    let gcode_link = document.getElementById( "gcode_link" );
    gcode_link
    if( gcode_link.childNodes.length == 1 && gcode_link.childNodes.item( 0 ) instanceof Text )
    {
      gcode_link.childNodes[0].data = filename;
    }
    else
    {
      gcode_link.childNodes.forEach( gcode_link.removeChild.bind( gcode_link ) );
      gcode_link.appendChild( document.createTextNode( filename ) );
    }
    gcode_link.href = url;
  }
} );


