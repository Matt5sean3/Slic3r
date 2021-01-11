
#include "thread.hpp"
#include <memory>

namespace compat {

void 
thread_group::add_thread( std::thread * thread_ptr )
{
    threads.push_back( std::move( * thread_ptr ) );
    delete thread_ptr;
}

void
thread_group::join_all( )
{
    for( auto & aThread : threads )
    {
        aThread.join( );
    }
}

void
this_thread::interruption_point( )
{
}

}

