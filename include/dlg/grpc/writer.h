#ifndef __DLG_GRPC_WRITER_HEADER__
#define __DLG_GRPC_WRITER_HEADER__

#include "operation.h"
#include "../util/flag.h"

#include <grpc++/support/async_stream.h>

#include <list>

namespace dlg
{
namespace grpc
{

template< class C, class W >
struct StreamWriterBase : public Operation
{
	void Store( W&& message )
	{
		queue.push_back( std::move(message) );
	}

	void Write( W&& );

	bool Proceed();

	std::list<W> queue;
};

template< class C, class Base >
struct Writer : public Base
{
	void Done( bool ok ) override;
};

} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_WRITER_HEADER__
