#ifndef __DLG_GRPC_CLIENT_WRITER_HEADER__
#define __DLG_GRPC_CLIENT_WRITER_HEADER__

#include "../operation.h"
#include "../writer.h"

namespace dlg
{
namespace grpc
{
namespace client
{

template< class C, class W >
struct WriterBase : public VoidOperation
{
};

template< class C, class W >
struct WriterBase< C, Stream<W> > : public StreamWriterBase<C, W>
{
	using Base = StreamWriterBase<C, W>;

	void Dismiss()
	{
		if( closing.Raise() && !Base::engaged && Base::queue.empty() )
		{
			Proceed();
		}
	}

	bool Proceed();

	Flag closing = false;
	Flag closed = false;
};

template< class C, class W >
struct WriterT
{
	struct Type : public VoidOperation
	{
	};
};

template< class C, class W >
struct WriterT< C, Stream<W> >
{
	using Type = grpc::Writer< C, WriterBase< C, Stream<W> > >;
};

template< class C, class W >
using Writer = typename WriterT< C, W >::Type;

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_WRITER_HEADER__
