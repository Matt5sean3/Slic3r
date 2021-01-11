
#ifndef compat_bind_hpp
#define compat_bind_hpp

#if NO_COMPILED_BOOST
    #include <functional>
#else
    #include <boost/bind/bind.hpp>
#endif

namespace compat {
#if NO_COMPILED_BOOST
    using std::bind;
    namespace placeholders
    {
        using namespace std::placeholders;
    }
#else
    using boost::bind;
    namespace placeholders
    {
        using namespace boost::placeholders;
    }
#endif
}

#endif // compat_bind_hpp
