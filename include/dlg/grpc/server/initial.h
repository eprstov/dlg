#ifndef __DLG_GRPC_SERVER_INITIAL_HEADER__
#define __DLG_GRPC_SERVER_INITIAL_HEADER__

#include "../operation.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< typename R, typename W > struct Initializer : public VoidOperation
{
};

template< typename R, typename W >
struct Initializer< Stream<R>, W > : public Operation
{
	void Done(bool) override;
};

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_INITIAL_HEADER__
