#ifndef __DLG_GRPC_SERVER_DIALOG_HEADER__
#define __DLG_GRPC_SERVER_DIALOG_HEADER__

#include "../../dialog.h"
#include "core.h"

#include <grpc++/grpc++.h>

#include <functional>

namespace dlg
{
namespace grpc
{
namespace server
{

template< typename R, typename W >
using CorePtr = std::unique_ptr< Core<R, W>, std::function< void( grpc::CoreBase* ) > >;

template< typename Rq, typename Rp, typename Tag >
class ProducerBase : public dlg::server::Dialog< Rq, Rp, Tag >
{
};

template< typename Rq, typename Rp, typename Tag >
class ProducerBase< Stream<Rq>, Rp, Tag > : public dlg::server::Dialog< Stream<Rq>, Rp, Tag >
{
protected:
	using Consumer = Consumer< Stream<Rq> >;

	void Attach( Consumer& cnsm ) override;
	void Detach( trait::ProducerTag<Consumer> ) override;
};

template< typename Rq, typename Rp, typename Tag >
class ConsumerBase : public ProducerBase< Rq, Rp, Tag >
{
};

template< typename Rq, typename Rp, typename Tag >
class ConsumerBase< Rq, Stream<Rp>, Tag > : public ProducerBase< Rq, Stream<Rp>, Tag >
{
	void Shut( trait::ConsumerTag<Rp> ) override;
};

template< class Rq, class Rp, typename Tag >
class Dialog : public ConsumerBase< Rq, Rp, Tag >, private grpc::CoreBase::Handler
{
	friend ProducerBase< Rq, Rp, Tag >;
	friend ConsumerBase< Rq, Rp, Tag >;

public:
	Dialog( CorePtr<Rq, Rp>&& _core, trait::Single<Rq>&& _rq ) : core( std::move(_core) ), rq( std::move(_rq) )
	{
		core->Notify( *this );
	}

	~Dialog()
	{
		if(core)
		{
		}
	}

	void operator<<( trait::Single<Rp> msg ) override
	{
		assert(core);
		core->writer().Write( std::move(msg) );
	}

	void Notify( Session::Handler hdlr ) override
	{
		assert( !handler );
		handler = hdlr;
	}

	trait::Single<Rq>&& request() override
	{
		return std::move(rq);
	}

	bool broken() const override
	{
		assert(core);
		return !core->running();
	}

private:
	void Updated( CoreBase::Tag ) override
	{
		if( !core->running() )
		{
			assert(handler);
			handler();
		}
	}

	CorePtr<Rq, Rp> core;
	trait::Single<Rq> rq;

	Session::Handler handler;
};

template< class Rq, class Rp, typename Tag >
void ProducerBase< Stream<Rq>, Rp, Tag >::Attach( Consumer& consumer )
{
	auto& core = *static_cast< Dialog< Stream<Rq>, Rp, Tag >* >(this)->core;
	core.Attach(consumer);
}

template< class Rq, class Rp, typename Tag >
void ProducerBase< Stream<Rq>, Rp, Tag >::Detach( trait::ProducerTag<Consumer> )
{
	auto& core = *static_cast< Dialog< Stream<Rq>, Rp, Tag >* >(this)->core;
	core.Detach();
}

template< class Rq, class Rp, typename Tag >
void ConsumerBase< Rq, Stream<Rp>, Tag >::Shut( trait::ConsumerTag<Rp> )
{
	auto& core = *static_cast< Dialog< Rq, Stream<Rp>, Tag >* >(this)->core;
	core.Dismiss();
}

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_DIALOG_HEADER__
