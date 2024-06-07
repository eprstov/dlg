#ifndef __DLG_LOCAL_DIALOG_HEADER__
#define __DLG_LOCAL_DIALOG_HEADER__

#include "../dialog.h"

#include <type_traits>

namespace dlg
{
namespace local
{
namespace client::detail
{

template< typename Rq, typename Rp, typename Tag > class DialogBase;

} // namespace client::detail
namespace server
{
namespace detail
{

template< typename Rq, typename Rp, typename Tag >
class DialogBase : public dlg::server::Dialog< Rq, Rp, Tag >
{
public:
	using Client = client::detail::DialogBase< Rq, Rp, Tag >;

	DialogBase( trait::Single<Rq>&& rq ) : req( std::move(rq) )
	{
	}

	~DialogBase() override;

	void Set( Client& _cli )
	{
		cli = &_cli;
	}

	void Reset()
	{
		cli = nullptr;
		if(handler) handler();
	}

	bool broken() const override
	{
		return !cli;
	}

	void operator<<( trait::Single<Rp> rp ) override;

	trait::Single<Rq>&& request() override
	{
		return std::move(req);
	}

	void Notify( Session::Handler hdlr ) override
	{
		assert( !handler );
		handler = std::move(hdlr);
	}

	trait::Single<Rq> req;
	Session::Handler handler;

	Client* cli = nullptr;
};

} // namespace detail

template< typename Rq, typename Rp, typename Tag >
class Dialog : public detail::DialogBase< Rq, Rp, Tag >
{
public:
	using detail::DialogBase< Rq, Rp, Tag >::DialogBase;
};

template< typename Rq, typename Rp, typename Tag >
class Dialog< Stream<Rq>, Rp, Tag > : public detail::DialogBase< Stream<Rq>, Rp, Tag >
{
public:
	using detail::DialogBase< Stream<Rq>, Rp, Tag >::DialogBase;

	using Consumer = dlg::Consumer< Stream<Rq> >;

	void Attach( Consumer& _cnsm ) override
	{
		assert( !cnsm );
		cnsm = &_cnsm;
	}

	void Detach( trait::ProducerTag<Rq> = {} ) override
	{
		assert(cnsm);
		cnsm = nullptr;
	}

	Consumer* consumer() { return cnsm; }

private:
	Consumer* cnsm = nullptr;
};

} // namespace server
namespace client
{

class Initiator;

namespace detail
{

template< class Rq, class Rp, typename Tag >
class DialogBase : public dlg::client::Dialog< Rq, Rp, Tag >
{
public:
	using Server = server::Dialog< Rq, Rp, Tag >;

	using Receiver = dlg::Consumer< std::unique_ptr< dlg::server::Dialog<Rq, Rp> >&& >;

	DialogBase( trait::Single<Rq>&& rq, Receiver& recv ) : request( std::move(rq) ), receiver( &recv )
	{
	}

	~DialogBase()
	{
		if(srv)
		{
			srv->Reset();
		}
	}

	void Set( Server& server )
	{
		assert( !server );
		srv = &server;
	}

	void Reset()
	{
		srv = nullptr;
		if(handler) handler();
	}

	bool broken() const override
	{
		return !srv;
	}

	std::unique_ptr<Server> Create()
	{
		auto server = std::make_unique<Server>( std::move(request) );

		srv = server.get();
		server->Set( *this );

		return server;
	}

	void Attach( Consumer<Rp>& consumer ) override
	{
		assert( !cnsm );
		cnsm = &consumer;

		if(receiver)
		{
			*receiver << Create();
			receiver = nullptr;
		}
	}

	void Detach( trait::ProducerTag<Rp> = {} ) override
	{
		assert(cnsm);
		cnsm = nullptr;
	}

	Consumer<Rp>* consumer() { return cnsm; }

	void Notify( Session::Handler hdlr ) override
	{
		assert( !handler );
		handler = hdlr;
	}

protected:
	trait::Single<Rq> request;
	Receiver* receiver;

	Session::Handler handler;

	Server* srv = nullptr;
	Consumer<Rp>* cnsm = nullptr;
};

} // namespace detail

template< typename Rq, typename Rp, typename Tag >
class Dialog : public detail::DialogBase< Rq, Rp, Tag >
{
public:
	using detail::DialogBase< Rq, Rp, Tag >::DialogBase;
	~Dialog() = default;
};

template< typename Rq, typename Rp, typename Tag >
class Dialog< Stream<Rq>, Rp, Tag > : public detail::DialogBase< Stream<Rq>, Rp, Tag >
{
	using Base = detail::DialogBase< Stream<Rq>, Rp, Tag >;
	using typename Base::Consumer;
	using Base::srv;

public:
	using Base::Base;

	void Attach( Consumer& cnsm ) override
	{
		Base::Attach(cnsm);

		while( !queue.empty() )
		{
			Rq& rq = queue.front();
			Push( std::move(rq) );

			queue.pop_front();
		}
	}

	void operator<<( Rq rq ) override
	{
		if(srv)
		{
			Push( std::move(rq) );
		}
		else
			queue.emplace_back( std::move(rq) );
	}

private:
	void Push( Rq&& rq )
	{
		auto* cnsm = srv->consumer();

		assert(cnsm);
		*cnsm << std::move(rq);
	}

	std::list<Rq> queue;
};

} // namespace client
namespace server::detail
{

template< typename Rq, typename Rp, typename Tag >
DialogBase< Rq, Rp, Tag >::~DialogBase()
{
	if(cli)
	{
		cli->Reset();
	}
}

template< typename Rq, typename Rp, typename Tag >
void DialogBase< Rq, Rp, Tag >::operator<<( trait::Single<Rp> rp )
{
	if(cli)
	{
		if( auto* cnsm = cli->consumer() )
		{
			*cnsm << std::move(rp);
		}
	}
}

} // namespace server::detail

template< class Rq, class Rp, typename Tag = void >
class Initiator : public dlg::server::Acceptor< dlg::server::Dialog< Rq, Rp, Tag > >, private Consumer< std::unique_ptr< dlg::server::Dialog< Rq, Rp, Tag > >&& >
{
public:
	Initiator() = default;
	Initiator( const Initiator& ) = delete;

	using Srv = dlg::server::Dialog< Rq, Rp, Tag >;

	using Consumer = dlg::Consumer< std::unique_ptr<Srv>&& >;

	std::unique_ptr< dlg::client::Dialog< Rq, Rp, Tag > > operator()( trait::Single<Rq> request )
	{
		dlg::Consumer< std::unique_ptr<Srv>&& >& rcv = *this;
		return std::make_unique< client::Dialog< Rq, Rp, Tag > >( std::move(request), rcv );
	}

	void operator<<( std::unique_ptr<Srv>&& server ) override
	{
		if(consumer)
		{
			*consumer << std::move(server);
		}
	}

	void Attach( Consumer& cnsm ) override
	{
		assert( !consumer );
		consumer = &cnsm;
	}

	void Detach( trait::ProducerTag<Consumer> = {} ) override
	{
		assert(consumer);
		consumer = nullptr;
	}

private:
	Consumer* consumer = nullptr;
};

} // namespace local
} // namespace dlg

#endif // __DLG_LOCAL_DIALOG_HEADER__
