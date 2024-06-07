#ifndef __DLG_GRPC_SERVER_WRITER_HEADER__
#define __DLG_GRPC_SERVER_WRITER_HEADER__

#include "../operation.h"
#include "../writer.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< class C, class W >
struct WriterBase : public Operation
{
	void Write( W&& );

	bool Proceed() { return false; }
};

template< class C, class W >
struct WriterBase< C, Stream<W> > : public StreamWriterBase<C, W>
{
};

template< class C, class W >
using Writer = grpc::Writer< C, WriterBase<C, W> >;

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_SERVER_WRITER_HEADER__
