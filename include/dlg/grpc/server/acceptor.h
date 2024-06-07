#ifndef __DLG_GRPC_SERVER_ACCEPTOR_HEADER__
#define __DLG_GRPC_SERVER_ACCEPTOR_HEADER__

#include "../operation.h"
#include "../factory.h"
#include "core.h"
#include "dialog.h"

#include <cassert>

namespace dlg
{
namespace grpc
{
namespace server
{

template< class R, class W, typename Tag >
class Acceptor : public dlg::server::Acceptor< Dialog< R, W, Tag > >, dlg::Consumer<R>, private grpc::CoreBase::Handler
{
public:
	using Core = server::Core<R, W>;
	using Factory = Factory::Unit<Core>;
	using Activator = server::Activator::Unit<R, W>;

	using Consumer = dlg::Consumer< std::unique_ptr< dlg::server::Dialog< R, W, Tag > >&& >;

	Acceptor( Factory&& fctr, Activator&& actr ) : factory( std::move(fctr) ), activator( std::move(actr) )
	{
	}

	~Acceptor()
	{
	}

	void Activate()
	{
		assert( !core );
		Renew();
	}

	void Deactivate()
	{
		assert(core);
		core.reset();
	}

	void Attach( Consumer& cnsm ) override
	{
		assert( !consumer );
		consumer = &cnsm;

		if(core)
		{
			Request();
		}
	}

	void Detach( trait::ProducerTag<Consumer> ) override
	{
		assert(consumer);
		consumer = nullptr;
	}

private:
	bool active() const { return core; }

	void Renew()
	{
		core = factory();
		if(consumer)
		{
			Request();
		}
	}

	void Request()
	{
		core->Attach( *this );
		core->Notify( *this );
		activator( *core );
	}

	void operator<<( trait::Single<R> request ) override
	{
		assert(consumer);
		if(consumer)
		{
			core->Detach( *this );

			auto dialog = std::make_unique< Dialog< R, W, Tag > >( std::move(core), std::move(request) );
			*consumer << std::move(dialog);
		}

		Renew();
	}

	void Updated( CoreBase::Tag ) override
	{
		if( !core->running() )
		{
			core.reset();
			Renew();
		}
	}

	trait::Single<R> request;
	CorePtr<R, W> core;

	Consumer* consumer = nullptr;

	Factory factory;
	Activator activator;
};

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_ACCEPTOR_HEADER__
