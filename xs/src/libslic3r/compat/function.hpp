
#ifndef compat_function_hpp
#define compat_function_hpp

#ifdef NO_COMPILED_BOOST
    #include <functional>
#else
    #include "boost/function.hpp"
#endif

namespace compat
{
#ifdef NO_COMPILED_BOOST
    using std::function;
#else
    using boost::function;
#endif
}

#endif

