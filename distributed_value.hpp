#pragma once

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/container/vector.hpp>
#include <boost/function.hpp>



namespace distributed
{
  template< class T > class proxy;
  template< class T > class source;

    namespace detail
    {
        template< class T >
        class proxy_data
        {
            T value;
            boost::container::vector< boost::function<void( const T& )> > callbacks;
        public:

            explicit proxy_data( T initial_value )
                : value( std::move( initial_value ) )
            {}

            void on_changed( T new_value )
            {
                value = std::move( new_value );
                for( auto&& callback : callbacks )
                    callback( new_value );
            }

            const T& get() const { return value; }
        };

        template< class T >
        using proxy_data_ptr = boost::shared_ptr<detail::proxy_data<T>>;
    }


    template< class T >
    class proxy
    {
    public:
        using source = source<T>;

        T get() const { return m_data->get(); }


    private:
        using data_ptr = detail::proxy_data_ptr<T>;
        data_ptr m_data;

        friend class source;

        explicit proxy(data_ptr data)
            : m_data(std::move(data))
        {}
    };



    template< class T >
    class source
    {
    public:
        using proxy = proxy<T>;

        template< class... InitArgs >
        source(InitArgs&&... args)
            : m_value(std::forward<InitArgs>(args)...)
        {}

        template< class Func >
        void write(Func&& func)
        {
            func(m_value);
            notify_change();
        }

        template< class Func >
        void operator()(Func&& func)
        {
            write(std::forward<Func>(func));
        }

        template< class U >
        source& operator=(U new_value)
        {
            write([=](T& value) { value = new_value; });
            return *this;
        }

        proxy make_proxy()
        {
            auto data = boost::make_shared< detail::proxy_data<T> >(m_value);
            m_proxies.emplace_back(data);
            return proxy(std::move(data));
        }

    private:

        T m_value;
        boost::container::vector< boost::weak_ptr< detail::proxy_data<T> > > m_proxies;


        void notify_change()
        {
            for(auto&& wp_data : m_proxies)
            {
                if(auto sp_data = wp_data.lock())
                {
                    sp_data->on_changed(m_value);
                }
            }
        }

    };


}

