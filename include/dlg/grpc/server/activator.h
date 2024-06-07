#ifndef __DLG_GRPC_SERVER_ACTIVATOR_HEADER__
#define __DLG_GRPC_SERVER_ACTIVATOR_HEADER__

#include "core.h"

#include <grpc++/grpc++.h>

namespace dlg
{
namespace grpc
{
namespace server
{

class Activator
{
public:
	template< typename R, typename W >
	using Unit = std::function< void( Core<R, W>& ) >;

	void Set( ServerQueue& q )
	{
		assert( !queue );
		queue = &q;
	}

	void Reset()
	{
		assert(queue);
		queue = nullptr;
	}

	template< class S, class R, class W >
	auto unit( S* svc, MethodRW<S, R, W> method ) -> Unit<R, W>
	{
		return [this, svc, method] ( Core<R, W>& core )
		{
			assert(queue);

			auto& ctx = core.context;
			auto& watcher = core.watcher();
			ctx.AsyncNotifyWhenDone( watcher.Engage() );

			auto& reader = core.reader();
			( svc->*method )( &core.context, &reader.message, &core.async(), queue, queue, reader.Engage() );
		};
	}

	template< class S, class R, class W >
	auto unit( S* svc, MethodRSW<S, R, W> method ) -> Unit< R, Stream<W> >
	{
		return [this, svc, method] ( Core< R, Stream<W> >& core )
		{
			assert(queue);

			auto& ctx = core.context;
			auto& watcher = core.watcher();
			ctx.AsyncNotifyWhenDone( watcher.Engage() );

			auto& reader = core.reader();
			( svc->*method )( &ctx, &reader.message, &core.async(), queue, queue, reader.Engage() );
		};
	}

	template< class S, class R, class W >
	auto unit( S* svc, MethodSRW<S, R, W> method ) -> Unit< Stream<R>, W >
	{
		return [this, svc, method] ( Core< Stream<R>, W >& core )
		{
			assert(queue);

			auto& ctx = core.context;
			auto& watcher = core.watcher();
			ctx.AsyncNotifyWhenDone( watcher.Engage() );

			auto& init = core.init();
			( svc->*method )( &core.context, &core.async(), queue, queue, init.Engage() );
		};
	}

	template< class S, class R, class W >
	auto unit( S* svc, MethodSRSW<S, R, W> method ) -> Unit< Stream<R>, Stream<W> >
	{
		return [this, svc, method] ( Core< Stream<R>, Stream<W> >& core )
		{
			assert(queue);

			auto& ctx = core.context;
			auto& watcher = core.watcher();
			ctx.AsyncNotifyWhenDone( watcher.Engage() );

			auto& init = core.init();
			( svc->*method )( &core.context, &core.async(), queue, queue, init.Engage() );
		};
	}

private:
	ServerQueue* queue = nullptr;
};

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_ACTIVATOR_HEADER__
