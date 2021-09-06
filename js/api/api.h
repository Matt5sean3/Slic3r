
#ifndef ModSlicer_API_H_
#define ModSlicer_API_H_

// This creates an API of byte layouts that should be implementable in varying languages

#include <emscripten/emscripten.h>
#include "Structured.h"

namespace ModSlicer
{
  // General messages
  typedef Structured::Message< std::string > Error;
  typedef Structured::Message< std::string > Progress;

  // === Initialize ===
  // Sets the number of threads
  typedef Structured::Message< > InitializationSuccess;
  typedef Structured::Message< int > InitializationRequest;

  enum class InitializationResponseMessage
  {
    ERROR,
    SUCCESS
  };

  typedef Structured::NamedMessageProtocol<
    Structured::Protocol<
      Error,
      InitializationSuccess >,
    InitializationResponseMessage > InitializationResponseProtocol;

  // === Slice ===
  typedef Structured::Message< std::string, std::string > SliceSuccess;
  typedef Structured::Message<
    std::vector< std::pair< std::string, std::string > >,
    std::vector< std::pair< std::string, std::string > > >
      SliceRequest;

  enum class SliceResponseMessage
  {
    ERROR,
    PROGRESS,
    SUCCESS,
    UNKNOWN
  };

  typedef Structured::NamedMessageProtocol<
    Structured::Protocol<
      Error,
      Progress,
      SliceSuccess >,
    SliceResponseMessage > SliceResponseProtocol;

  // === LIST SETTINGS ===
  // Define the settings listing protocol
  typedef Structured::Message< > ListSettingsRequest;

  enum class SettingType
  { // For now, corresponds one-to-one to ConfigOptionType in Slic3r
    NONE,
    FLOAT,
    FLOATS,
    INT,
    INTS,
    STRING,
    STRINGS,
    PERCENT,
    FLOAT_OR_PERCENT,
    POINT,
    POINTS,
    POINT3,
    BOOL,
    BOOLS,
    ENUM
  };

  typedef Structured::Message<
    std::vector< std::string > // Just a list of all the names of the settings
      > ListSettingsSuccess;

  enum class ListSettingsResponseMessage
  {
    ERROR,
    SUCCESS
  };

  typedef Structured::NamedMessageProtocol<
    Structured::Protocol<
      Error,
      ListSettingsSuccess >,
    ListSettingsResponseMessage > ListSettingsResponseProtocol;


  // === REQUEST ===

  enum class RequestMessage
  {
    INITIALIZE,
    SLICE,
    LIST_SETTINGS,
    UNKNOWN
  };

  typedef Structured::NamedMessageProtocol<
    Structured::Protocol<
      InitializationRequest,
      SliceRequest,
      ListSettingsRequest >,
    RequestMessage > RequestProtocol;

} // ModSlicer


#endif
