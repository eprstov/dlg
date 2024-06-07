#ifndef __DLG_GRPC_CLIENT_FINAL_INLINE__
#define __DLG_GRPC_CLIENT_FINAL_INLINE__

#include "../final.cpp.h"

#include <dlg/util/log.h>

namespace dlg
{
namespace grpc
{
namespace client
{

template< typename R, typename W >
inline
void Finalizer<R, W>::DoneReading()
{
	auto& core = *static_cast< Core< R, W>* >(this);
	core.Done();
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, W >::Finish()
{
	auto& core = *static_cast< Core< Stream<R>, W>* >(this);

	auto& reader = core.reader();
	if( reader.engaged )
	{
		auto& ctx = core.context;
		return ctx.TryCancel();
	}
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, W >::DoneReading()
{
	auto& core = *static_cast< Core< Stream<R>, W>* >(this);
	log << "finler " << &core << " Finish()" << std::endl;

	auto& async = core.async();
	async.Finish( &status, Base::Engage() );

	if( !core.dismissed )
	{
		core.Shut();

		core.Broke();
		core.Notify();
	}
}

template< typename R, typename W >
inline
void Finalizer< R, Stream<W> >::Finish()
{
	auto& core = *static_cast< Core< R, Stream<W> >* >(this);

	auto& writer = core.writer();
	writer.Dismiss();
}

template< typename R, typename W >
inline
void Finalizer< R, Stream<W> >::DoneWriting()
{
	auto& core = *static_cast< Core< R, Stream<W> >* >(this);
	log << "reader " << &core << " Finish()" << std::endl;

	auto& async = core.async();
	auto& reader = core.reader();
	async.Finish( &reader.status, reader.Engage() );
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, Stream<W> >::Finish()
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);

	auto& writer = core.writer();
	writer.Dismiss();

	auto& reader = core.reader();
	if( reader.engaged && !writer.engaged )
	{
		auto& ctx = core.context;
		ctx.TryCancel();
	}
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, Stream<W> >::DoneReading()
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);

	if( finalized.Raise() )
	{
		log << "finler " << &core << " Finish()" << std::endl;

		auto& async = core.async();
		async.Finish( &status, Base::Engage() );
	}

	if( !core.dismissed )
	{
		core.Shut();

		core.Broke();
		core.Notify();
	}
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, Stream<W> >::DoneWriting()
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);

	if( core.dismissed )
	{
		auto& reader = core.reader();
		if( reader.engaged )
		{
			auto& ctx = core.context;
			return ctx.TryCancel();
		}

		return DoneReading();
	}

	core.Done();
}

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_FINAL_HEADER__
