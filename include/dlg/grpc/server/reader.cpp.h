#ifndef __DLG_GRPC_SERVER_READER_INLINE__
#define __DLG_GRPC_SERVER_READER_INLINE__

#include "../core.h"

#include "../reader.cpp.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< class C, typename R >
inline
void ReaderBase<C, R>::Read( R& )
{
	auto& core = *static_cast< C* >(this);
	core.issued.Raise();
}

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_READER_INLINE__
