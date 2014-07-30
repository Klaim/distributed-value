#include <boost/enable_shared_from_this.hpp>


namespace util
{
    template< class T >
    class distributed
    {
    public:

        template< class... InitArgs >
        distributed( InitArgs... args )
            : m_value( std::forward<InitArgs>(args) )
        {}

        distributed( const distributed& ) = delete;
        distributed& operator=( const distributed& ) = delete;

        distributed( const distributed& ) = delete;
        distributed& operator=( const distributed& ) = delete;

        template< class T >
        class proxy
        {
            T m_value;
        public:

            proxy(  )


        };





    private:
        T value;



    };




}

