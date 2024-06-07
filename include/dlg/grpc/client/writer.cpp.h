#ifndef __DLG_GRPC_CLIENT_WRITER_INLINE__
#define __DLG_GRPC_CLIENT_WRITER_INLINE__

#include "writer.h"
#include "../writer.cpp.h"

namespace dlg
{
namespace grpc
{
namespace client
{

template< class C, class W >
inline
bool WriterBase< C, Stream<W> >::Proceed()
{
	Base::Proceed();
	if( !Base::engaged & closing )
	{
		const auto done = !closed.Raise();
		if( !done )
		{
			auto& core = *static_cast< C* >(this);
			log << "writer " << &core << " WritesDone()" << std::endl;

			auto& async = core.async();
			async.WritesDone( Base::Engage() );
		}

		return done;
	}

	return true;
}

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_WRITER_INLINE__
