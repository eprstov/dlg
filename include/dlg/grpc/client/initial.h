#ifndef __DLG_GRPC_CLIENT_INITIAL_HEADER__
#define __DLG_GRPC_CLIENT_INITIAL_HEADER__

#include "../operation.h"
#include "../../basic.h"

namespace dlg
{
namespace grpc
{
namespace client
{

template< typename R, typename W > struct Initializer : public VoidOperation
{
};

template< typename R, typename W >
struct Initializer< Stream<R>, W > : public Operation
{
	void Done(bool) override;
};

template< typename R, typename W >
struct Initializer< R, Stream<W> > : public Operation
{
	void Done(bool) override;
};

template< typename R, typename W >
struct Initializer< Stream<R>, Stream<W> > : public Operation
{
	void Done(bool) override;
};

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_INITIAL_HEADER__
