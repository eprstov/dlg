#ifndef __DLG_GRPC_FACTORY_HEADER__
#define __DLG_GRPC_FACTORY_HEADER__

#include "supervisor.h"

#include <type_traits>

namespace dlg
{
namespace grpc
{

class Factory : private Supervisor
{
public:

	template< typename Core >
	using CorePtr = std::unique_ptr< Core, std::function< void( grpc::CoreBase* ) > >;

	template< typename Core >
	using Unit = std::function< CorePtr<Core>() >;

	template< typename Core >
	Unit<Core> unit()
	{
		static_assert( std::is_base_of_v< CoreBase, Core > );
		static_assert( std::is_default_constructible_v<Core> );

		return [this]()
		{
			auto core = new Core();
			auto&& retire = [this, token = Add(core)] (grpc::CoreBase*)
			{
				Supervisor::Retire(token);
			};

			return CorePtr<Core>( core, std::move(retire) );
		};
	}

	using Supervisor::active;
	using Supervisor::Drop;
	using Supervisor::Cleanup;
};

} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_FACTORY_HEADER__
