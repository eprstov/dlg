#ifndef __DLG_GRPC_CLIENT_ACTIVATOR_HEADER__
#define __DLG_GRPC_CLIENT_ACTIVATOR_HEADER__

#include "core.h"

#include <grpc++/grpc++.h>

#include <functional>

namespace dlg
{
namespace grpc
{
namespace client
{

class Activator
{
public:
	template< typename R, typename W >
	using Unit = std::function< void( Core<R, W>&, trait::Single<W>&& ) >;

	Activator( ::grpc::CompletionQueue& que ) : queue(que)
	{
	}

	template< class S, class W, class R >
	auto unit( S* svc, MethodWR<S, W, R> method ) -> Unit<R, W>
	{
		return [this, svc, method] ( Core<R, W>& core, W&& request )
		{
			auto& async = core.prop.async;
			async = (svc->*method)( &core.context, request, &queue );

			auto& reader = core.reader();
			async->Finish( &reader.message, &reader.status, reader.Engage() );
		};
	}

	template< class S, class W, class R >
	auto unit( S* svc, MethodWSR<S, W, R> method ) -> Unit< Stream<R>, W >
	{
		return [this, svc, method] ( Core< Stream<R>, W >& core, W&& request )
		{
			auto& init = core.init();
			core.prop.async = (svc->*method)( &core.context, request, &queue, init.Engage() );
		};
	}

	template< class S, class W, class R >
	auto unit( S* svc, MethodSWR<S, W, R> method ) -> Unit< R, Stream<W> >
	{
		return [this, svc, method] ( Core< R, Stream<W> >& core, W&& request )
		{
			auto& init = core.init();
			auto& reader = core.reader();
			core.prop.async = (svc->*method)( &core.context, &reader.message, &queue, init.Engage() );

			auto& writer = core.writer();
			writer.Store( std::move(request) );
		};
	}

	template< class S, class W, class R >
	auto unit( S* svc, MethodSWSR<S, W, R> method ) -> Unit< Stream<R>, Stream<W> >
	{
		return [this, svc, method] ( Core< Stream<R>, Stream<W> >& core, W&& request )
		{
			auto& init = core.init();
			core.prop.async = (svc->*method)( &core.context, &queue, init.Engage() );

			auto& writer = core.writer();
			writer.Store( std::move(request) );
		};
	}

private:
	::grpc::CompletionQueue& queue;
};

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_ACTIVATOR_HEADER__
