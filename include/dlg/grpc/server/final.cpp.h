#ifndef __DLG_GRPC_SERVER_FINAL_INLINE__
#define __DLG_GRPC_SERVER_FINAL_INLINE__

#include "../final.cpp.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< typename R, typename W >
inline
void Finalizer<R, W>::Finish()
{
	auto& core = *static_cast< Core<R, W>* >(this);

	auto& reader = core.reader();
	if( reader.engaged )
	{
		auto& ctx = core.context;
		ctx.TryCancel();
	}

	if( finalized.Raise() )
	{
		log << "writer " << &core << " Finish()" << std::endl;

		auto& async = core.async();
		auto& writer = core.writer();
		async.FinishWithError( ::grpc::Status::CANCELLED, writer.Engage() );
	}
}

template< typename R, typename W >
inline
void Finalizer< R, Stream<W> >::Finish()
{
	auto& core = *static_cast< Core< R, Stream<W> >* >(this);

	auto& reader = core.reader();
	if( reader.engaged )
	{
		auto& ctx = core.context;
		ctx.TryCancel();
	}

	auto& writer = core.writer();
	if( !writer.engaged )
	{
		DoneWriting();
	}
}

template< typename R, typename W >
inline
void Finalizer< R, Stream<W> >::DoneWriting()
{
	auto& core = *static_cast< Core< R, Stream<W> >* >(this);
	log << "finler " << &core << " Finish()" << std::endl;

	auto& async = core.async();
	async.Finish( ::grpc::Status::OK, Base::Engage() );
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, W >::Finish()
{
	auto& core = *static_cast< Core< Stream<R>, W >* >(this);

	auto& init = core.init();
	if( init.engaged )
	{
		return core.Broke();
	}

	auto& reader = core.reader();
	if( reader.engaged )
	{
		auto& ctx = core.context;
		ctx.TryCancel();
	}

	if( finalized.Raise() )
	{
		log << "writer " << &core << " Finish()" << std::endl;

		auto& async = core.async();
		auto& writer = core.writer();
		async.FinishWithError( ::grpc::Status::CANCELLED, writer.Engage() );
	}
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, W >::DoneReading()
{
	auto& core = *static_cast< Core< Stream<R>, W >* >(this);

	if( !core.dismissed )
	{
		core.Shut();
	}

	core.Done();
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, W >::DoneWriting()
{
	auto& core = *static_cast< Core< Stream<R>, W >* >(this);
	core.Done();
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, Stream<W> >::Finish()
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);

	auto& init = core.init();
	if( init.engaged )
	{
		return core.Broke();
	}

	auto& reader = core.reader();
	if( reader.engaged )
	{
		auto& ctx = core.context;
		ctx.TryCancel();
	}

	auto& writer = core.writer();
	if( !writer.engaged )
	{
		DoneWriting();
	}
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, Stream<W> >::DoneReading()
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);

	if( !core.dismissed )
	{
		return core.Shut();
	}

	core.Done();
}

template< typename R, typename W >
inline
void Finalizer< Stream<R>, Stream<W> >::DoneWriting()
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);
	log << "finler " << &core << " Finish()" << std::endl;

	auto& async = core.async();
	auto& writer = core.writer();
	async.Finish( ::grpc::Status::OK, Base::Engage() );
}

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_FINAL_INLINE__
