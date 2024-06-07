#ifndef __DLG_GRPC_CLIENT_TRAITS_HEADER__
#define __DLG_GRPC_CLIENT_TRAITS_HEADER__

#include "../../dialog.h"

namespace dlg::grpc::client
{

template<class S, class W, class R>
using MethodWR = std::unique_ptr< ::grpc::ClientAsyncResponseReader<R> > (S::*) ( ::grpc::ClientContext*, const W&, ::grpc::CompletionQueue* );

template<class S, class W, class R>
using MethodWSR = std::unique_ptr< ::grpc::ClientAsyncReader<R> > (S::*) ( ::grpc::ClientContext*, const W&, ::grpc::CompletionQueue*, void* );

template<class S, class W, class R>
using MethodSWR = std::unique_ptr< ::grpc::ClientAsyncWriter<W> > (S::*) ( ::grpc::ClientContext*, R*, ::grpc::CompletionQueue*, void* );

template<class S, class W, class R>
using MethodSWSR = std::unique_ptr< ::grpc::ClientAsyncReaderWriter<W, R> > (S::*) ( ::grpc::ClientContext*, ::grpc::CompletionQueue*, void* );

namespace detail
{

template< class W, class R >
struct Method
{
	template< class S > using Type = MethodWR< S, W, R >;
};

template< class W, class R >
struct Method< W, Stream<R> >
{
	template< class S > using Type = MethodWSR< S, W, R >;
};

template< class W, class R >
struct Method< Stream<W>, R >
{
	template< class S > using Type = MethodSWR< S, W, R >;
};

template< class W, class R >
struct Method< Stream<W>, Stream<R> >
{
	template< class S > using Type = MethodSWSR< S, W, R >;
};

template< class W, class R >
struct Async
{
	using Type = ::grpc::ClientAsyncResponseReader<R>;
};

template< class W, class R >
struct Async< W, Stream<R> >
{
	using Type = ::grpc::ClientAsyncReader<R>;
};

template< class W, class R >
struct Async< Stream<W>, R >
{
	using Type = ::grpc::ClientAsyncWriter<W>;
};

template< class W, class R >
struct Async< Stream<W>, Stream<R> >
{
	using Type = ::grpc::ClientAsyncReaderWriter<W, R>;
};

} // namespace detail

template< class Svc, class W, class R > using Method = typename detail::Method<W, R>::template Type<Svc>;

template< class W, class R > using Async = typename detail::Async<W, R>::Type;

template< typename S, typename W, typename R >
struct Trait
{
	using Rq = W;
	using Rp = R;
	using Svc = S;

	using Async = Async<Rq, Rp>;
	template< typename Tag > using Dialog = dlg::client::Dialog< Rq, Rp, Tag >;
	template< typename Tag > using Initiator = ::dlg::client::Initiator< Dialog<Tag> >;
};

} // namespace dlg::grpc::client

#endif // __DLG_GRPC_CLIENT_TRAITS_HEADER__
