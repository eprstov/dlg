#ifndef __DLG_GRPC_SUPERVISOR_HEADER__
#define __DLG_GRPC_SUPERVISOR_HEADER__

#include "core.h"

#include <cassert>
#include <list>
#include <set>

namespace dlg
{
namespace grpc
{

class Supervisor
{
	using Cores = std::list< CoreBase* >;

public:
	using Token = Cores::iterator;

	~Supervisor()
	{
		assert( !active() );
	}

	void Drop()
	{
		while( !engaged.empty() )
		{
			Retire( engaged.begin(), true );
		}
	}

	void Cleanup()
	{
		assert( engaged.empty() );
		for( auto& item : retired )
		{
			item.Force();
		}
	}

	Token Add( CoreBase* core )
	{
		assert( std::find( engaged.begin(), engaged.end(), core ) == engaged.end() );
		return engaged.insert( engaged.end(), core );
	}

	void Retire( Token it, bool force = false )
	{
		auto* core = *it;
		engaged.erase(it);

		if( !force )
		{
			core->Dismiss();
		}

		if( core->engaged() )
		{
			if(force)
			{
				core->Abort();
			}
			core->Notify( Attend( *core ) );
			// TODO: cancel by timeout
		}
		else
			delete core;
	}

	bool active() const { return !engaged.empty() || !retired.empty(); }

protected: // TODO: redesign
	struct Attendant;

	using Attendants = std::list<Attendant> ;

	struct Attendant : public CoreBase::Handler
	{
		Attendant( Attendants& atts, CoreBase& c ) : attendants(atts), core( &c ) {}
		virtual ~Attendant()
		{
			delete core; // TODO: replace with unique_ptr
		}

		void Force()
		{
			core->Broke();
			core->Abort(); // TODO: check why assert( engaged() );
		}

		void Updated( CoreBase::Tag ) override
		{
			if( !core->engaged() )
			{
				// TODO: enqueue
				attendants.erase(it);
			}
		}

		Attendants::iterator it;
		Attendants& attendants;

		CoreBase* core;
	};

	Attendant& Attend( CoreBase& core )
	{
		auto it = retired.emplace( retired.end(), retired, core );
		it->it = it;

		return *it;
	}

	Cores engaged;
	Attendants retired;
};

} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SUPERVISOR_HEADER__
