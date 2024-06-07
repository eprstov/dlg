#ifndef __DLG_GRPC_TRAITS_HEADER__
#define __DLG_GRPC_TRAITS_HEADER__

#include "server/traits.h"
#include "client/traits.h"

namespace dlg::grpc
{
namespace detail
{

template< typename Method > struct Trait;

template< class S, class R, class W > struct Trait< server::MethodRW<S, R, W> > : server::Trait< S, R, W >
{
};

template< class S, class R, class W > struct Trait< server::MethodRSW<S, R, W> > : server::Trait< S, R, Stream<W> >
{
};

template< class S, class R, class W > struct Trait< server::MethodSRW<S, R, W> > : server::Trait< S, Stream<R>, W >
{
};

template< class S, class R, class W > struct Trait< server::MethodSRSW<S, R, W> > : server::Trait< S, Stream<R>, Stream<W> >
{
};

template< typename S, typename W, typename R > struct Trait< client::MethodWR<S, W, R> > : client::Trait< S, W, R >
{
};

template< typename S, typename W, typename R > struct Trait< client::MethodWSR<S, W, R> > : client::Trait< S, W, Stream<R> >
{
};

template< typename S, typename W, typename R > struct Trait< client::MethodSWR<S, W, R> > : client::Trait< S, Stream<W>, R >
{
};

template< typename S, typename W, typename R > struct Trait< client::MethodSWSR<S, W, R> > : client::Trait< S, Stream<W>, Stream<R> >
{
};

} // namespace detail

template< typename Method > using Trait = detail::Trait<Method>;

namespace trait
{
using namespace dlg::trait;

template< typename Method > using Rq = typename Trait<Method>::Rq;
template< typename Method > using Rp = typename Trait<Method>::Rp;
template< typename Method > using Svc = typename Trait<Method>::Svc;

} // namespace trait
} // namespace dlg::grpc

#endif // __DLG_GRPC_TRAITS_HEADER__
