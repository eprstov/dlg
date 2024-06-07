#ifndef __DLG_GRPC_SERVER_INITIAL_INLINE__
#define __DLG_GRPC_SERVER_INITIAL_INLINE__

#include "initial.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< typename R, typename W >
inline
void Initializer< Stream<R>, W >::Done( bool ok )
{
	auto& core = *static_cast< Core< Stream<R>, W >* >(this);

	if( !ok )
	{
		core.Broke();
	}

	if( core.broken )
	{
		return core.Done();
	}

	core.issued.Raise();

	auto& reader = core.reader();
	reader.Read();
}

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_INITIAL_INLINE__
