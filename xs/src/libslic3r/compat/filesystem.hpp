
#ifdef NO_COMPILED_BOOST
  #include <filesystem>
#else
  #include <boost/filesystem.hpp>
#endif

// A near one-to-one switch
namespace compat
{
  namespace filesystem {
#ifdef NO_COMPILED_BOOST
    using namespace std::filesystem;
#else
    using namespace boost::filesystem;
#endif
  }
}

