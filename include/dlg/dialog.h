#ifndef __DLG_DIALOG_HEADER__
#define __DLG_DIALOG_HEADER__

#include "basic.h"

#include <gain/cast.h>

#include <memory>

namespace dlg
{

struct Session
{
	using Handler = std::function< void() >;
	virtual void Notify(Handler) = 0;

	virtual bool broken() const = 0;
};

namespace server
{

template< class Rq, class Rp, typename Tag = void > struct Dialog;

template< class Rq, class Rp >
struct Dialog< Rq, Rp, void > : public Session, public Producer<void>, public Consumer<Rp>
{
	virtual Rq&& request() = 0;
	virtual ~Dialog() {}
};

template< class Rq, class Rp >
struct Dialog< Stream<Rq>, Rp, void > : public Session, public Producer< Stream<Rq> >, public Consumer<Rp>
{
	virtual Rq&& request() = 0;
	virtual ~Dialog() {}
};

template< class Rq, class Rp, typename Tag > struct Dialog : Dialog< Rq, Rp, void >
{
};

template< typename T >
using Acceptor = Producer< std::unique_ptr< gain::template_cast< Dialog, T > >&& >;

} // namespace server
namespace client
{

template< class Rq, class Rp, typename Tag = void > struct Dialog;

template< class Rq, class Rp >
struct Dialog< Rq, Rp, void > : public Session, public Consumer<void>, public Producer<Rp>
{
	virtual ~Dialog() {}
};

template< class Rq, class Rp >
struct Dialog< Stream<Rq>, Rp, void > : public Session, public Consumer< Stream<Rq> >, public Producer<Rp>
{
	virtual ~Dialog() {}
};

template< class Rq, class Rp, typename Tag > struct Dialog : Dialog< Rq, Rp, void >
{
};

namespace detail
{

template<typename> struct Initiator;

template< typename Rq, typename Rp >
struct Initiator< Dialog< Rq, Rp > >
{
	using Type = std::unique_ptr< Dialog< Rq, Rp > >( trait::Single<Rq> );
};

} // namespace detail

template< typename T >
using Initiator = std::function< typename detail::Initiator< gain::template_cast< Dialog, T > >::Type >;

} // namespace client
} // namespace dlg

#endif // __DLG_DIALOG_HEADER__
