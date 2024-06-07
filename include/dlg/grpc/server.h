#ifndef __DLG_GRPC_SERVER_HEADER__
#define __DLG_GRPC_SERVER_HEADER__

#include "factory.h"
#include "traits.h"

#include "server/activator.h"
#include "server/core.h"
#include "server/dialog.h"
#include "server/service.h"

#include <grpc++/grpc++.h>

namespace dlg
{
namespace grpc
{

template <class... T> class ServerT {};

class ServerBase
{
protected:
	~ServerBase()
	{
		// order matters
		_queue.reset();
		instance.reset();
	}

	using Queue = ::grpc::CompletionQueue;
	using ServerQueue = ::grpc::ServerCompletionQueue;

	template< class... T > class Starter;

	std::unique_ptr<ServerQueue> _queue;
	std::unique_ptr< ::grpc::Server > instance;

	server::Activator activator;
	Factory factory;

public:
	bool active() const { return factory.active(); }
	ServerQueue& queue() { return *_queue; }
};

template< class... T > class ServerBase::Starter
{
	using Driven = ServerT<T...>;
	friend Driven;

private:
	Starter( Driven& _driven ) : driven( _driven ) {}

public:
	void Start( const std::string& address ) const
	{
		::grpc::ServerBuilder builder;

		builder.AddListeningPort( address, ::grpc::InsecureServerCredentials() );
		driven.Register( builder );

		driven._queue = builder.AddCompletionQueue();
		driven.instance = builder.BuildAndStart();

		driven.activator.Set( *driven._queue );

		driven.Activate();
	}

	void Pause() const
	{
		driven.Deactivate();
	}

	void Stop() const
	{
		driven.factory.Cleanup();

		driven.instance->Shutdown();
		driven._queue->Shutdown();
	}

private:
	Driven& driven;
};

template< class Svc >
struct ServerS
{
	ServerS( Factory& factory, server::Activator& activator ) : service( factory, activator )
	{
	}

	void Register( ::grpc::ServerBuilder& builder )
	{
		builder.RegisterService( &service );
	}

	template< auto mtd >
	requires std::is_convertible_v< Svc&, server::ServiceM<mtd>& >
	auto& acceptor()
	{
		return static_cast< server::ServiceM<mtd>& >(service).acceptor;
	}

	Svc service;
};

template<>
class ServerT<> : public ServerBase
{
public:

	void Activate()
	{
	}

	void Deactivate()
	{
	}

	template<typename>
	constexpr void acceptor()
	{
	}

	void Register( auto& )
	{
	}
};

template< class S, class... Tail >
class ServerT< S, Tail... > : public ServerT< Tail... >, public ServerS<S>
{
	using Base = ServerT< Tail... >;
	using BaseS = ServerS<S>;
	using AsyncService = typename S::AsyncService;

public:
	ServerT() : BaseS( ServerBase::factory, ServerBase::activator )
	{
	}

	using ServerS<S>::acceptor;
	using Base::acceptor;

	using Starter = ServerBase::Starter< S, Tail... >;
	friend Starter;

	Starter starter() { return Starter( *this ); }

	using ServerBase::queue;
	using ServerBase::active;

	void Register( ::grpc::ServerBuilder& builder )
	{
		BaseS::Register(builder);
		Base::Register(builder);
	}

	void Activate()
	{
		BaseS::service.Activate();
		Base::Activate();
	}

	void Deactivate()
	{
		Base::Deactivate();
		BaseS::service.Deactivate();
	}

	using Base::_queue;
	using Base::instance;
};

namespace detail::server
{

template< typename > struct Binder;

template< template< auto... > typename Pack >
struct Binder< Pack<> >
{
	static void Attach( auto&, auto& ) {}
	static void Detach( auto&, auto& ) {}
};

template< template < auto... > typename Pack, auto mtd, auto... mtds >
struct Binder< Pack< mtd, mtds... > > : Binder< Pack< mtds... > >
{
	using Base = Binder< Pack< mtds... > >;

	template< typename Server, typename Pool >
	requires requires (Server server, Pool pool)
	{
		server.template acceptor<mtd>().Attach(pool);
	}
	static void Attach( Server& server, Pool& pool )
	{
		auto& acpt = server.template acceptor<mtd>();
		acpt.Attach(pool);

		Base::Attach( server, pool );
	}

	template< typename Server, typename Pool >
	requires requires (Server server, Pool pool)
	{
		server.template acceptor<mtd>().Detach(pool);
	}
	static void Detach( Server& server, Pool& pool )
	{
		auto& acpt = server.template acceptor<mtd>();
		acpt.Detach(pool);

		Base::Detach( server, pool );
	}
};

template< auto... > struct Pack;

} // namespace detail::server

template< typename... Services >
class Server : public ServerT< Services... >
{
public:
	template< typename Pack, typename Pool >
	void Attach( Pool& pool )
	{
		detail::server::Binder<Pack>::Attach( *this, pool );
	}

	template< typename Pack, typename Pool >
	void Detach( Pool& pool )
	{
		detail::server::Binder<Pack>::Detach( *this, pool );
	}

	template< auto... mtds, typename Pool >
	void Attach( Pool& pool )
	{
		using namespace detail::server;
		Binder< Pack< mtds... > >::Attach( *this, pool );
	}

	template< auto... mtds, typename Pool >
	void Detach( Pool& pool )
	{
		using namespace detail::server;
		Binder< Pack< mtds... > >::Detach( *this, pool );
	}
};

} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_SERVER_HEADER__
