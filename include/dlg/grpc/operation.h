#ifndef __DLG_GRPC_OPERATION_HEADER__
#define __DLG_GRPC_OPERATION_HEADER__

#include <cassert>
#include <functional>

namespace dlg
{
namespace grpc
{

struct Operation
{
	using Tag = void*;

	virtual void Done(bool) = 0;

	Tag Engage()
	{
		assert( !engaged );
		engaged = true;

		return this;
	}

	void Handle( bool ok )
	{
		assert( engaged );
		engaged = false;

		return Done(ok);
	}

	static void Handle( Tag tag, bool ok )
	{
		auto* operation = reinterpret_cast< Operation* >(tag);
		return operation->Handle(ok);
	}

	bool engaged = false;
};

struct VoidOperation
{
	static constexpr bool engaged = false;
};

} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_OPERATION_HEADER__
