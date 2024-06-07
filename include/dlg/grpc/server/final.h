#ifndef __DLG_GRPC_SERVER_FINAL_HEADER__
#define __DLG_GRPC_SERVER_FINAL_HEADER__

#include "../final.h"

#include "../../util/flag.h"

namespace dlg
{
namespace grpc
{
namespace server
{

template< typename R, typename W > struct Core;
template< typename R, typename W > using FinalizerBase = grpc::FinalizerBase< Core<R, W> >;

template< typename R, typename W > struct Finalizer : public VoidOperation
{
	void Finish();
	void DoneReading() {}
	void DoneWriting() {}

	Flag finalized = false;
};

template< typename R, typename W > struct Finalizer< R, Stream<W> > : public FinalizerBase< R, Stream<W> >
{
	using Base = FinalizerBase< R, Stream<W> >;

	void Finish();
	void DoneWriting();
};

template< typename R, typename W > struct Finalizer< Stream<R>, W > : public VoidOperation
{
	void Finish();
	void DoneReading();
	void DoneWriting();

	Flag finalized = false;
};

template< typename R, typename W > struct Finalizer< Stream<R>, Stream<W> > : public FinalizerBase< Stream<R>, Stream<W> >
{
	using Base = FinalizerBase< Stream<R>, Stream<W> >;

	void Finish();
	void DoneReading();
	void DoneWriting();
};

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_SERVER_FINAL_HEADER__
