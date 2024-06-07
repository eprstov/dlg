#ifndef __DLG_GRPC_FINAL_INLINE__
#define __DLG_GRPC_FINAL_INLINE__

#include "final.h"

namespace dlg
{
namespace grpc
{

template< typename C >
inline
void FinalizerBase<C>::Done( bool ok )
{
	grpc::CoreBase& core = *static_cast< C* >(this);
	log << "finler " << &core << " Done(" << (ok ? "true" : "false") << ")" << std::endl;

	return core.Done();
}

template< typename C >
inline
void FinalizerBase<C>::DoneReading()
{
	grpc::CoreBase& core = *static_cast< C* >(this);
	if( !core.running() )
	{
		return core.Done();
	}
}

template< typename C >
inline
void FinalizerBase<C>::DoneWriting()
{
	grpc::CoreBase& core = *static_cast< C* >(this);
	if( !core.running() )
	{
		return core.Done();
	}
}

} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_FINAL_INLINE__
