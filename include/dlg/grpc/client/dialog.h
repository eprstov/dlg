#ifndef __DLG_GRPC_CLIENT_DIALOG_HEADER__
#define __DLG_GRPC_CLIENT_DIALOG_HEADER__

#include "../../dialog.h"

#include "core.h"

#include <grpc++/grpc++.h>

namespace dlg
{
namespace grpc
{
namespace client
{

template< typename R, typename W >
using CorePtr = std::unique_ptr< Core<R, W>, std::function< void( grpc::CoreBase* ) > >;

template< typename Rq, typename Rp >
class DialogBase : public dlg::client::Dialog<Rq, Rp>
{
};

template< typename Rq, typename Rp >
class DialogBase< Stream<Rq>, Rp > : public dlg::client::Dialog< Stream<Rq>, Rp >
{
public:
	void operator<<( Rq request ) override;
	void Shut( trait::ConsumerTag<Rq> ) override;
};

template< typename Rq, typename Rp >
class Dialog : public DialogBase<Rq, Rp>, private grpc::CoreBase::Handler
{
public:
	using Base = DialogBase<Rq, Rp>;
	friend Base;

	using A = Async<Rq, Rp>;
	using C = Core<Rp, Rq>;

	using Handler = grpc::CoreBase::Handler;
	using Consumer = dlg::Consumer<Rp>;

	Dialog( CorePtr<Rp, Rq>&& c ) : core( std::move(c) )
	{
		core->Notify( *this );
	}

	~Dialog()
	{
	}

	void Updated( CoreBase::Tag ) override
	{
		if( !core->running() )
		{
			assert(handler);
			handler();
		}
	}

	void Attach( Consumer& consumer ) override
	{
		assert(core);
		core->Attach(consumer);
	}

	void Detach( trait::ProducerTag<Consumer> ) override
	{
		assert(core);
		core->Detach();
	}

	bool broken() const override
	{
		assert(core);
		return !core->running();
	}

	void Notify( Session::Handler hdlr ) override
	{
		assert( !handler );
		handler = std::move(hdlr);
	}

private:
	CorePtr<Rp, Rq> core;
	Session::Handler handler;
};

template< typename Rq, typename Rp >
inline
void DialogBase< Stream<Rq>, Rp >::operator<<( Rq request )
{
	auto& core = *static_cast< Dialog< Stream<Rq>, Rp>* >(this)->core;
	core.Write( std::move(request) );
}

template< typename Rq, typename Rp >
inline
void DialogBase< Stream<Rq>, Rp >::Shut( trait::ConsumerTag<Rq> )
{
	auto& core = *static_cast< Dialog< Stream<Rq>, Rp>* >(this)->core;

	auto& writer = core.writer();
	writer.Dismiss();
}

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_DIALOG_HEADER__
