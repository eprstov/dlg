#ifndef __DLG_GRPC_SERVER_TRAITS_HEADER__
#define __DLG_GRPC_SERVER_TRAITS_HEADER__

#include "../../dialog.h"

#include <grpc++/grpc++.h>

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace dlg::grpc::server
{

using Queue = ::grpc::CompletionQueue;
using ServerQueue = ::grpc::ServerCompletionQueue;

template<class S, class R, class W>
using MethodRW = void (S::*) ( ::grpc::ServerContext*, R*, ::grpc::ServerAsyncResponseWriter<W>*, Queue*, ServerQueue*, void* );

template<class S, class R, class W>
using MethodRSW = void (S::*) ( ::grpc::ServerContext*, R*, ::grpc::ServerAsyncWriter<W>*, Queue*, ServerQueue*, void* );

template<class S, class R, class W>
using MethodSRW = void (S::*) ( ::grpc::ServerContext*, ::grpc::ServerAsyncReader<W, R>*, Queue*, ServerQueue*, void* );

template<class S, class R, class W>
using MethodSRSW = void (S::*) ( ::grpc::ServerContext*, ::grpc::ServerAsyncReaderWriter<W, R>*, Queue*, ServerQueue*, void* );

namespace detail
{

template< class R, class W >
struct Method
{
	template< class S > using Type = MethodRW< S, R, W >;
};

template< class R, class W >
struct Method< R, Stream<W> >
{
	template< class S > using Type = MethodRSW< S, R, W >;
};

template< class R, class W >
struct Method< Stream<R>, W >
{
	template< class S > using Type = MethodSRW< S, R, W >;
};

template< class R, class W >
struct Method< Stream<R>, Stream<W> >
{
	template<class S> using Type = MethodSRSW< S, R, W >;
};

template< class R, class W >
struct Async
{
	using Type = ::grpc::ServerAsyncResponseWriter<W>;
};

template< class R, class W >
struct Async< R, Stream<W> >
{
	using Type = ::grpc::ServerAsyncWriter<W>;
};

template< class R, class W >
struct Async< Stream<R>, W >
{
	using Type = ::grpc::ServerAsyncReader< W, R >;
};

template< class R, class W >
struct Async< Stream<R>, Stream<W> >
{
	using Type = ::grpc::ServerAsyncReaderWriter< W, R >;
};

} // namespace detail

template< class Svc, class R, class W >
using Method = typename detail::Method< R, W >::template Type<Svc>;

template< class R, class W > using Async = typename detail::Async< R, W >::Type;

template< typename S, typename R, typename W >
struct Trait
{
	using Rq = R;
	using Rp = W;
	using Svc = S;

	using Async = Async<Rq, Rp>;
	template< typename Tag > using Dialog = ::dlg::server::Dialog< Rq, Rp, Tag >;
	template< typename Tag > using Acceptor = dlg::server::Acceptor< Dialog<Tag> >;
};

} // namespace dlg::grpc::server

#endif // __DLG_GRPC_SERVER_TRAITS_HEADER__
