#ifndef __DLG_GRPC_SERVER_WRITER_INLINE__
#define __DLG_GRPC_SERVER_WRITER_INLINE__

#include "writer.h"
#include "../writer.cpp.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< class C, class W >
inline
void WriterBase<C, W>::Write( W&& message )
{
	auto& core = *static_cast< C* >(this);

	if( core.finalized.Raise() )
	{
		auto& async = core.async();
		async.Finish( message, ::grpc::Status::OK, Engage() );
	}
}

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_WRITER_INLINE__
