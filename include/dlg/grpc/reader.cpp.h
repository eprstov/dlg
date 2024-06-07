#ifndef __DLG_GRPC_READER_INLINE__
#define __DLG_GRPC_READER_INLINE__

#include "core.h"
#include "reader.h"

#include <dlg/util/log.h>

namespace dlg
{
namespace grpc
{

template< class C, typename R >
inline
void ReaderBase< C, Stream<R> >::Read( R& message )
{
	auto& core = *static_cast< C* >(this);
	log << "reader " << &core << " Read()" << std::endl;

	auto& async = core.async();
	async.Read( &message, Engage() );
}

template< typename Base >
inline
void Reader<Base>::Done( bool ok )
{
	Core& core = *static_cast< Core* >(this);
	log << "reader " << &core << " Done(" << (ok ? "true" : "false") << ")" << std::endl;

	if( !Base::Verify() )
	{
		core.Broke();
	}

	if( core.broken )
	{
		return core.Done();
	}

	if( ok && !core.dismissed )
	{
		Base::Read(message);
		core.Forward( std::move(message) );
	}
	else
		core.DoneReading();
}

} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_READER_INLINE__

