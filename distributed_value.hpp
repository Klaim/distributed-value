#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/container/vector.hpp>
#include <boost/function.hpp>


namespace util
{
    template< class T >
    class distributed
    {
        private:

        class proxy_data
        {
            T value;
            boost::container::vector< boost::function<void( const T& )> > callbacks;
        public:

            explicit proxy_data( T initial_value )
                : value( std::move( initial_value ) ) {}

            void on_changed( T new_value )
            {
                value = std::move( new_value );
                for( auto&& callback : callbacks )
                    callback( new_value );
            }

            const T& get() const { return value; }
        };


    public:

        
        class proxy
        {
        public:

            T get() const { return m_data->get(); }


        private:
            boost::shared_ptr<proxy_data> m_data;
            friend class source;

            explicit proxy( boost::shared_ptr<proxy_data> data )
                : m_data( std::move( data ) )
            {}
        };

        class source
        {
        public:

            template< class... InitArgs >
            source( InitArgs&&... args )
                : m_value( std::forward<InitArgs>( args )... )
            {}

            template< class Func >
            void write( Func&& func )
            {
                func( m_value );
                notify_change();
            }

            template< class Func >
            void operator()( Func&& func )
            {
                write( std::forward<Func>(func) );
            }

            template< class U >
            source& operator=( U new_value )
            {
                write( [=]( T& value ) { value = new_value; } );
                return *this;
            }

            proxy make_proxy()
            {
                auto data = boost::make_shared<proxy_data>( m_value );
                m_proxies.emplace_back( data );
                return proxy( std::move( data ) );
            }

        private:

            T m_value;
            boost::container::vector< boost::weak_ptr<proxy_data> > m_proxies;


            void notify_change()
            {
                for( auto&& wp_data : m_proxies )
                {
                    if( auto sp_data = wp_data.lock() )
                    {
                        sp_data->on_changed( m_value );
                    }
                }
            }

        };


        

    
    };




}

