#ifndef __DLG_GRPC_SERVER_SERVICE_HEADER__
#define __DLG_GRPC_SERVER_SERVICE_HEADER__

#include "acceptor.h"
#include "activator.h"

#include "../traits.h"

#include <type_traits>

namespace dlg::grpc
{

template <class... T> class Server;

namespace server
{

template< auto... > struct MethodPack;
template< auto mtd, typename Tag > requires std::is_member_function_pointer_v< decltype(mtd) > struct Tagged {};

template< class S, auto... methods > class Service;

namespace detail::trait
{
using namespace grpc::trait;

template< auto mtd > struct TagT
{
	using Type = void;
};

template< auto mtd, typename Tag, Tagged< mtd, Tag > tagged > struct TagT<tagged>
{
	using Type = Tag;
};

template< auto mtd > using Tag = typename TagT<mtd>::Type;

template< auto mtd > constexpr auto method = nullptr;
template< auto mtd > requires std::is_member_function_pointer_v< decltype(mtd) > static constexpr auto method<mtd> = mtd;
template< auto mtd, typename Tag, Tagged< mtd, Tag > tagged > constexpr auto method<tagged> = mtd;

template< auto mtd > using Method = std::remove_cvref_t< decltype( method<mtd> ) >;

} // namespace detail::trait

template< auto mtd >
struct ServiceM
{
	using Method = detail::trait::Method<mtd>;

	using Rq = trait::Rq<Method>;
	using Rp = trait::Rp<Method>;
	using Svc = trait::Svc<Method>;

	ServiceM( Factory& factory, Activator& activator, Svc* svc ) : acceptor( factory.unit< Core<Rq, Rp> >(), activator.unit( svc, detail::trait::method<mtd> ) )
	{
	}

	Acceptor< trait::Rq<Method>, trait::Rp<Method>, detail::trait::Tag<mtd> > acceptor;
};

template< class S >
class Service<S> : public S::AsyncService
{
public:
	Service( auto&, auto& )
	{
	}

	void Activate()
	{
	}

	void Deactivate()
	{
	}

	using AsyncService = typename S::AsyncService;
};

template< class Svc, auto mtd, auto... mtds >
class Service< Svc, mtd, mtds... > : public Service< Svc, mtds... >, public ServiceM<mtd>
{
public:
	using Base = Service< Svc, mtds... >;
	using BaseM = ServiceM<mtd>;

	using Methods = MethodPack< mtd, mtds... >;

	Service( Factory& factory, Activator& activator ) : Base( factory, activator ), BaseM( factory, activator, this )
	{
	}

	void Activate()
	{
		BaseM::acceptor.Activate();
		Base::Activate();
	}

	void Deactivate()
	{
		BaseM::acceptor.Deactivate();
		Base::Deactivate();
	}

	static_assert( std::is_base_of_v< trait::Svc< detail::trait::Method<mtd> >, typename Svc::AsyncService > );

	using AsyncService = typename Svc::AsyncService;
};

} // namespace server
} // namespace dlg::grpc

#endif // __DLG_GRPC_SERVER_SERVICE_HEADER__
