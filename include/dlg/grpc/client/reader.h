#ifndef __DLG_GRPC_CLIENT_READER_HEADER__
#define __DLG_GRPC_CLIENT_READER_HEADER__

#include "../reader.h"

namespace dlg
{
namespace grpc
{
namespace client
{

template< class C, typename R >
struct ReaderBase : public grpc::ReaderBase<C, R>
{
	using Core = C;
	using Message = R;

	void Read( R& ) {}

	bool ok() const { return status.ok(); }
	bool Verify() const { return status.ok(); }

	::grpc::Status status = ::grpc::Status::OK;
	trait::Single<R> message;
};

template< class C, typename R >
struct ReaderBase< C, Stream<R> > : public grpc::ReaderBase< C, Stream<R> >
{
};

template< class C, typename R >
using Reader = grpc::Reader< ReaderBase<C, R> >;

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_READER_HEADER__
