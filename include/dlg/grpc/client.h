#ifndef __DLG_GRPC_CLIENT_HEADER__
#define __DLG_GRPC_CLIENT_HEADER__

#include "traits.h"

#include "client/activator.h"
#include "client/core.h"
#include "client/dialog.h"
#include "factory.h"

#include <grpc++/grpc++.h>

namespace dlg
{
namespace grpc
{

class ClientBase
{
	template<typename> friend struct ClientS;

protected:
	using ChannelPtr = std::shared_ptr< ::grpc::ChannelInterface >;

	ClientBase( ChannelPtr chnl ) : activator(que), channel( std::move(chnl) )
	{
	}

	~ClientBase()
	{
	}

	::grpc::CompletionQueue que;

	client::Activator activator;
	Factory factory;

	ChannelPtr channel; // TODO: consider removing

public:
	void Activate()
	{
	}

	void Deactivate()
	{
		factory.Cleanup();
		que.Shutdown();
	}

	bool active() const
	{
		return factory.active();
	}

	::grpc::CompletionQueue& queue()
	{
		return que;
	}
};

template< class S >
struct ClientS
{
	using Stub = typename S::Stub;
	using ChannelPtr = std::shared_ptr< ::grpc::ChannelInterface >;

	ClientS( const ChannelPtr& chnl, Factory& fctr, client::Activator& actr ) : stub( S::NewStub(chnl) ), factory(fctr), activator(actr)
	{
	}

	template< typename M > using Svc = trait::Svc<M>;
	template< typename M > using Initiator = dlg::client::Initiator< typename Trait<M>::Dialog >;

	template< typename M >
	requires std::is_same_v< Svc<M>, Stub >
	auto initiator( M mtd )
	{
		using Rq = trait::Rq<M>;
		using Rp = trait::Rp<M>;
		using Core = client::Core<Rp, Rq>;

		return [factory = factory.unit<Core>(), activator = activator.unit( stub.get(), mtd )] (trait::Single<Rq> request)
		{
			auto core = factory();
			activator( *core, std::move(request) );

			return std::make_unique< client::Dialog<Rq, Rp> >( std::move(core) );
		};
	}

	std::unique_ptr<Stub> stub;

	Factory& factory;
	client::Activator& activator;
};

template< class ... > class Client;

template< class S >
class Client<S> : public ClientBase, public ClientS<S>
{
	using Base = ClientBase;
	using BaseS = ClientS<S>;

public:

	Client( ChannelPtr chnl ) : Base( std::move(chnl) ), BaseS( channel, factory, activator )
	{
	}

	using BaseS::initiator;
};

template< class S, class... Tail >
class Client< S, Tail... > : public Client< Tail... >, public ClientS<S>
{
	using Base = Client< Tail... >;
	using BaseS = ClientS<S>;

protected:
	using Base::channel;

public:
	using ChannelPtr = std::shared_ptr< ::grpc::ChannelInterface >;

	Client( ChannelPtr chnl ) : Base( std::move(chnl) ), BaseS( Base::channel, ClientBase::factory, ClientBase::activator )
	{
	}

	using Base::initiator;
	using BaseS::initiator;

	using Base::Activate;
	using Base::Deactivate;
};

} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_HEADER__
