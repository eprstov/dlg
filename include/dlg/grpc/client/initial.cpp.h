#ifndef __DLG_GRPC_CLIENT_INITIAL_INLINE__
#define __DLG_GRPC_CLIENT_INITIAL_INLINE__

namespace dlg
{
namespace grpc
{
namespace client
{

template< typename R, typename W >
inline
void Initializer< Stream<R>, W >::Done( bool ok )
{
	auto& core = *static_cast< Core< Stream<R>, W >* >(this);

	if( !ok )
	{
		core.Broke();
		return core.Done();
	}

	if( core.dismissed )
	{
		return core.DoneReading();
	}

	auto& reader = core.reader();
	return reader.Read();
}

template< typename R, typename W >
inline
void Initializer< R, Stream<W> >::Done( bool ok )
{
	auto& core = *static_cast< Core< R, Stream<W> >* >(this);

	if( !ok )
	{
		core.Broke(true);
		return core.Done();
	}

	auto& writer = core.writer();
	writer.Proceed();
}

template< typename R, typename W >
inline
void Initializer< Stream<R>, Stream<W> >::Done( bool ok )
{
	auto& core = *static_cast< Core< Stream<R>, Stream<W> >* >(this);

	if( !ok )
	{
		core.Broke();
		return core.Done();
	}

	auto& writer = core.writer();
	writer.Proceed();

	assert( writer.engaged );

	if( !core.dismissed )
	{
		auto& reader = core.reader();
		reader.Read();
	}
}

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_INITIAL_INLINE__
