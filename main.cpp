#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>


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
    a.request_end();
    return 0;
}

