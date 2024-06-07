#ifndef __DLG_HUB_HEADER__
#define __DLG_HUB_HEADER__

#include "basic.h"

#include <functional>
#include <list>
#include <memory>

namespace dlg
{

class Hub
{
public:
	template< template< typename, typename, typename > typename Dlg, typename Rq, typename Rp, typename Tag >
	void Add( std::unique_ptr< Dlg< Rq, Rp, Tag > >&&, std::function< bool( trait::Single<Rp>&& ) > handler );
	void Clear()
	{
		receivers.clear();
	}

private:
	template< template< typename, typename, typename > typename Dlg, typename Rq, typename Rp, typename Tag >
	struct Receiver;

	std::list< std::unique_ptr< dlg::Destructible<void> > > receivers;
};

template< template< typename, typename, typename > typename Dlg, typename Rq, typename Rp, typename Tag >
struct Hub::Receiver : private dlg::Consumer<Rp>, public dlg::Session, public dlg::Destructible<void>
{
	using Dialog = Dlg< Rq, Rp, Tag >;
	using Handler = std::function< void( trait::Single<Rp>&& ) >;

	Receiver( std::unique_ptr<Dialog>&& dlg ) : dialog( std::move(dlg) )
	{
		dialog->Attach( *this );
	}

	~Receiver()
	{
		dialog->Detach( *this );
	}

	void Notify( Handler&& hdlr )
	{
		handler = std::move(hdlr);
	}

	void Notify( Session::Handler handler ) override
	{
		return dialog->Notify( std::move(handler) );
	}

	bool broken() const override
	{
		return dialog->broken();
	}

private:
	void operator<<( trait::Single<Rp> message ) override
	{
		handler( std::move(message) );
	}

	std::unique_ptr<Dialog> dialog;
	Handler handler;
};


template< template< typename, typename, typename > typename Dlg, typename Rq, typename Rp, typename Tag >
void Hub::Add( std::unique_ptr< Dlg< Rq, Rp, Tag > >&& dlg, std::function< bool( trait::Single<Rp>&& ) > hdlr )
{
	using Receiver = Hub::Receiver< Dlg, Rq, Rp, Tag >;

	auto it = receivers.insert( receivers.end(), std::make_unique<Receiver>( std::move(dlg) ) );

	auto&& handler = [this, it, handler = std::move(hdlr)] (trait::Single<Rp>&& response)
	{
		if( !handler( std::move(response) ) )
		{
			receivers.erase(it);
		}
	};

	auto&& watcher = [this, it]
	{
		auto& receiver = static_cast< Receiver& >( **it );
		if( receiver.broken() )
		{
			receivers.erase(it);
		}
	};

	auto& receiver = static_cast< Receiver& >( **it );
	receiver.Notify( std::move(handler) );

	auto& session = static_cast< dlg::Session& >(receiver);
	session.Notify( std::move(watcher) );
}

} // namespace dlg

#endif // __DLG_HUB_HEADER__
