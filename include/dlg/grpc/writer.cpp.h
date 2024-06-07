#ifndef __DLG_GRPC_WRITER_INLINE__
#define __DLG_GRPC_WRITER_INLINE__

#include "operation.h"
#include "writer.h"

#include <grpc++/support/async_stream.h>

namespace dlg
{
namespace grpc
{

template< class C, class W >
inline
void StreamWriterBase<C, W>::Write( W&& message )
{
	auto& core = *static_cast< C* >(this);
	assert( !core.dismissed );

	if( !engaged )
	{
		log << "writer " << &core << " Write()" << std::endl;

		auto& async = core.async();
		async.Write( message, Engage() );
	}
	else
		queue.push_back( std::move(message) );
}

template< class C, class W >
inline
bool StreamWriterBase<C, W>::Proceed()
{
	auto& core = *static_cast< C* >(this);
	if( !queue.empty() )
	{
		log << "writer " << &core << " Write()" << std::endl;

		auto& async = core.async();
		async.Write( queue.front(), Engage() );

		queue.pop_front();
		return true;
	}

	return !core.dismissed;
}

template< class C, class Base >
inline
void Writer< C, Base >::Done( bool ok )
{
	C& core = *static_cast< C* >(this);
	log << "writer " << &core << " Done(" << (ok ? "true" : "false") << ")" << std::endl;

	if( !ok )
	{
		core.Broke();
	}

	if( core.broken )
	{
		return core.Done();
	}

	if( !Base::Proceed() )
	{
		return core.DoneWriting();
	}
}

} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_WRITER_INLINE__
