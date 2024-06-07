#ifndef __DLG_GRPC_SERVER_CORE_HEADER__
#define __DLG_GRPC_SERVER_CORE_HEADER__

#include "final.h"
#include "initial.h"
#include "reader.h"
#include "traits.h"
#include "watcher.h"
#include "writer.h"

#include "../core.h"
#include "../socket.h"

#include <dlg/util/flag.h>
#include <dlg/util/log.h>

#include <functional>

namespace dlg
{
namespace grpc
{
namespace server
{

template< typename R, typename W > struct Core;

struct CoreBase : public grpc::CoreBase
{
	virtual ~CoreBase() = default;
	void OnAbort() override;

	Flag issued = false;
	::grpc::ServerContext context;
};

template< typename R, typename W >
struct Core : public CoreBase, public Reader< Core<R, W>, R >, public Writer< Core<R, W>, W >, public Initializer<R, W>, public Finalizer<R, W>, public Watcher< Core<R, W> >, public Socket<R>
{
	using Rdr = Reader< Core, R >;
	using Wtr = Writer< Core, W >;
	using Init = Initializer<R, W>;
	using Fnl = Finalizer<R, W>;
	using Sock = Socket<R>;
	using Wtch = Watcher<Core>;

	Core() : prop(context) {}

	~Core()
	{
		assert( !engaged() );
	}

	using Sock::Attach;
	using Sock::Detach;

	using grpc::CoreBase::Done;

	Rdr& reader() { return *this; }
	Wtr& writer() { return *this; }
	Init& init() { return *this; }
	Fnl& finalizer() { return *this; }
	Wtch& watcher() { return *this; }

	void OnDismiss() override;

	bool engaged() const override
	{
		// TODO: remove the exception when https://github.com/grpc/grpc/issues/10136 gets fixed
		return Rdr::engaged || Wtr::engaged || Init::engaged || Fnl::engaged || (issued && Wtch::engaged);
	}

	Async< R, W >& async() { return prop.async; }

	struct Prop
	{
		Prop( ::grpc::ServerContext& context ) : async( &context ) {}

		Async< R, W > async;
	}
	prop;
};

inline void CoreBase::OnAbort()
{
	if(issued)
	{
		context.TryCancel();
	}
}

template< typename R, typename W >
inline
void Core<R, W>::OnDismiss()
{
	if(issued)
	{
		Fnl::Finish();
	}
}

} // namespace server
} // namespace grpc
} // namespace dlg

#include "final.cpp.h"
#include "initial.cpp.h"
#include "reader.cpp.h"
#include "watcher.cpp.h"
#include "writer.cpp.h"

#endif // __DLG_GRPC_SERVER_CORE_HEADER__
