#ifndef __DLG_GRPC_SERVER_WATCHER_INLINE__
#define __DLG_GRPC_SERVER_WATCHER_INLINE__

#include "watcher.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< class C >
inline
void Watcher<C>::Done( bool ok )
{
	auto& core = *static_cast< C* >(this);
	log << "watch " << &core << " Done(" << (ok ? "true" : "false" ) << ")" << std::endl;

	if( ok && core.issued )
	{
		return core.Broke(true);
	}

	core.Done();
}

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_SERVER_WATCHER_INLINE__
