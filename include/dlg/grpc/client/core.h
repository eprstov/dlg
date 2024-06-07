#ifndef __DLG_GRPC_CLIENT_CORE_HEADER__
#define __DLG_GRPC_CLIENT_CORE_HEADER__

#include "initial.h"
#include "final.h"
#include "reader.h"
#include "traits.h"
#include "writer.h"

#include "../core.h"
#include "../socket.h"

#include <functional>
#include <memory>

namespace dlg
{
namespace grpc
{
namespace client
{

template< typename R, typename W > struct Core;

struct CoreBase : public grpc::CoreBase
{
	void OnAbort() override;

	::grpc::ClientContext context;
};

template< typename R, typename W >
struct Core : public CoreBase, public Reader< Core<R, W>, R >, public Writer< Core<R, W>, W >, public Initializer<R, W>, public Socket<R>, public Finalizer<R, W>
{
	using Rdr = Reader< Core, R >;
	using Wtr = Writer< Core, W >;
	using Init = Initializer<R, W>;
	using Fnl = Finalizer<R, W>;
	using Sock = Socket<R>;

	Core() {}

	~Core() override
	{
		assert( !engaged() );
	}

	void Write( trait::Single<W>&& rq )
	{
		if( Init::engaged )
		{
			Wtr::Store( std::move(rq) );
		}
		else
			Wtr::Write( std::move(rq) );
	}

	using grpc::CoreBase::Done;
	using grpc::CoreBase::Dismiss;

	Rdr& reader() { return *this; }
	Wtr& writer() { return *this; }
	Init& init() { return *this; }

	void OnDismiss() override;

	bool engaged() const override
	{
		return Rdr::engaged || Wtr::engaged || Fnl::engaged || Init::engaged;
	}

	Async<W, R>& async()
	{
		return *prop.async;
	}

	void async( std::unique_ptr< Async<W, R> >&& async )
	{
		prop.async = std::move(async);
	}

	struct
	{
		std::unique_ptr< Async<W, R> > async;
	}
	prop;
};

inline void CoreBase::OnAbort()
{
	context.TryCancel();
}

template< typename R, typename W >
inline
void Core<R, W>::OnDismiss()
{
	Fnl::Finish();
}

} // namespace client
} // namespace grpc
} // namespace dlg

#include "final.cpp.h"
#include "initial.cpp.h"
#include "reader.cpp.h"
#include "writer.cpp.h"

#endif // __DLG_GRPC_CLIENT_CORE_HEADER__
