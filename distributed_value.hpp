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
    public:

        template< class... InitArgs >
        distributed( InitArgs... args )
            : m_value( std::forward<InitArgs>(args)... )
        {}

        template< class Func >
        void change( Func func )
        {
            func( m_value );
            notify_change();
        }

    private:

        class proxy_data
        {
            T value;
            boost::container::vector< boost::function<void( const T& )> > callbacks;
        public:

            explicit proxy_data( T initial_value )
                : value( std::move(initial_value) ) {}

            void on_changed( T new_value )
            {
                value = std::move(new_value);
                for( auto&& callback : callbacks )
                    callback( new_value );
            }

            const T& get() const { return value; }
        };


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


    public:

        template< class T >
        class proxy
        {
        public:

            T get() const { return m_data->get(); }


        private:
            boost::shared_ptr<proxy_data> m_data;
            friend class distributed<T>;

            explicit proxy( boost::shared_ptr<proxy_data> data )
                : m_data( std::move(data) )
            {}
        };

        proxy<T> make_proxy()
        {
            auto data = boost::make_shared<proxy_data>( m_value );
            m_proxies.emplace_back( data );
            return proxy<T>( std::move(data) );
        }

    };




}

