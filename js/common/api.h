
#ifndef ModSlicer_API_H_
#define ModSlicer_API_H_

#include <emscripten/emscripten.h>
#include "Structured.h"

namespace ModSlicer
{
  // General messages
  typedef Structured::Message< std::string > Error;
  typedef Structured::Message< std::string > Progress;

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

  enum class ListSettingsResponseMessage
  {
    ERROR,
    SUCCESS
  };

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

  typedef Structured::NamedMessageProtocol<
    Structured::Protocol<
      Error,
      ListSettingsSuccess >,
    ListSettingsResponseMessage > ListSettingsResponseProtocol;


  // === REQUEST ===

  enum class RequestMessage
  {
    SLICE,
    LIST_SETTINGS,
    UNKNOWN
  };

  typedef Structured::NamedMessageProtocol<
    Structured::Protocol<
      SliceRequest,
      ListSettingsRequest >,
    RequestMessage > RequestProtocol;
} // ModSlicer


#endif
