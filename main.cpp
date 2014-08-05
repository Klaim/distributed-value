#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>

#include "distributed_value.hpp"

class System_A
{
public:

    System_A()
        : m_running( true )
        , m_thread( [&]{ update_loop(); } )
    {}

    ~System_A()
    {
        m_thread.join();
    }

    void request_end() { m_running = false; }

private:

    boost::atomic<bool> m_running;
    boost::thread m_thread;

    void update_loop()
    {
        while( m_running )
            update_cycle();
    }

    void update_cycle()
    {

    }
};


int main()
{
    System_A a;

    util::distributed<int>::source source( 42 );
    auto proxy = source.make_proxy();
    assert( proxy.get() == 42 );

    source( []( int& value ){ value = 99; } );

    assert( proxy.get() == 99 );

    a.request_end();
    return 0;
}

