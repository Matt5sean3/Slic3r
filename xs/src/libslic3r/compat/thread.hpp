
#ifndef compat_thread_hpp
#define compat_thread_hpp

#ifdef NO_COMPILED_BOOST
    #include <thread>
    #include <mutex>
    #include <vector>
#else
    #include <boost/thread.hpp>
#endif

namespace compat
{
#ifdef NO_COMPILED_BOOST
    using std::mutex;
    using std::thread;
    using std::lock_guard;
    namespace this_thread
    {
        using namespace std::this_thread;
        // add interruption point function as a nop
        void interruption_point( );
    }

    class thread_group {
    private:
        std::vector< thread > threads;
    public:
        void add_thread( std::thread * thread_ptr );
        void join_all( );
    };
#else
    using boost::mutex;
    using boost::thread;
    using boost::lock_guard;
    using boost::thread_group;
    namespace this_thread
    {
        using namespace boost::this_thread;
    }
#endif
}

#endif // compat_thread_hpp

