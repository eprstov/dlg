#ifndef __DLG_GRPC_SERVER_READER_HEADER__
#define __DLG_GRPC_SERVER_READER_HEADER__

#include "../reader.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< class C, typename R >
struct ReaderBase : public grpc::ReaderBase<C, R>
{
	void Read( R& );
};

template< class C, typename R >
struct ReaderBase< C, Stream<R> > : public grpc::ReaderBase< C, Stream<R> >
{
};

template< class C, typename R >
using Reader = grpc::Reader< ReaderBase<C, R> >;

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_READER_HEADER
