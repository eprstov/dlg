#ifndef __DLG_GRPC_SOCKET_HEADER__
#define __DLG_GRPC_SOCKET_HEADER__

#include "../basic.h"
#include "../dialog.h"

namespace dlg
{
namespace grpc
{

template< typename R > struct SocketBase
{
	void Attach( Consumer<R>& cnsm )
	{
		assert( !consumer );
		consumer = &cnsm;
	}

	void Detach( trait::ProducerTag<R> = {} )
	{
		assert(consumer);
		consumer = nullptr;
	}

	void Forward( trait::Single<R>&& message )
	{
		assert(consumer);
		*consumer << std::move(message);
	}

	Consumer<R>* consumer = nullptr;
};

template< typename R >
struct Socket : public SocketBase<R>
{
};

template< typename R >
struct Socket< Stream<R> > : public SocketBase< Stream<R> >
{
	void Shut()
	{
		using Base = SocketBase< Stream<R> >;
		Base::consumer->Shut();
	}
};

} // namespace grpc
} // namespace dlg


#endif // !__DLG_GRPC_SOCKET_HEADER__
