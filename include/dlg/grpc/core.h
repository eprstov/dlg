#ifndef __DLG_GRPC_CORE_HEADER__
#define __DLG_GRPC_CORE_HEADER__

#include <cassert>
#include <functional>

#include "../util/flag.h"

#include <dlg/util/log.h>

namespace dlg
{
namespace grpc
{

struct CoreBase
{
	struct Tag {};
	struct Handler
	{
		virtual void Updated(Tag) = 0;
	};

	virtual ~CoreBase()
	{
		log << "core " << this << " destroyed" << std::endl;
	}

	void Notify( Handler& hdlr )
	{
		handler = &hdlr;
	}

	virtual bool engaged() const = 0;

	bool running() const { return !broken && !dismissed; }

	void Broke( bool notify = false )
	{
		if( broken.Raise() && notify )
		{
			Notify();
		}
	}

	void Done()
	{
		if( !running() )
		{
			Notify();
		}
	}

	virtual void OnAbort() = 0; // TODO: replace with static binding
	virtual void OnDismiss() = 0; // TODO: replace with static binding

	void Dismiss( bool notify = false )
	{
		if( dismissed.Raise() )
		{
			log << "core " << this << " dismissed" << std::endl;
			OnDismiss();

			if(notify)
			{
				Notify();
			}
		}
	}

	void Abort()
	{
		assert( engaged() );
		broken = true;

		return OnAbort();
	}

	Flag broken = false;
	Flag dismissed = false;

	void Notify()
	{
		assert(handler);
		handler->Updated( Tag() );
	}

	Handler* handler = nullptr;
};

} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CORE_HEADER__
