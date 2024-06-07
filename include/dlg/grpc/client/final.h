#ifndef __DLG_GRPC_CLIENT_FINAL_HEADER__
#define __DLG_GRPC_CLIENT_FINAL_HEADER__

#include "../final.h"
#include "../../util/flag.h"

namespace dlg
{
namespace grpc
{
namespace client
{

template< typename R, typename W > struct Core;
template< typename R, typename W > using FinalizerBase = grpc::FinalizerBase< Core<R, W> >;

template< typename R, typename W > struct Finalizer : public VoidOperation//: public FinalizerBase<R, W>
{
	void Finish() {}
	void DoneReading();
};

template< typename R, typename W > struct Finalizer< R, Stream<W> > : public VoidOperation //: public FinalizerBase< R, Stream<W> >
{
	using Base = FinalizerBase< R, Stream<W> >;

	void Finish();
	void DoneReading() {}
	void DoneWriting();
};

template< typename R, typename W > struct Finalizer< Stream<R>, W > : public FinalizerBase< Stream<R>, W >
{
	using Base = FinalizerBase< Stream<R>, W >;

	void Finish();
	void DoneReading();

	::grpc::Status status = ::grpc::Status::OK;
};

template< typename R, typename W > struct Finalizer< Stream<R>, Stream<W> > : public FinalizerBase< Stream<R>, Stream<W> >
{
	using Base = FinalizerBase< Stream<R>, Stream<W> >;

	void Finish();
	void DoneReading();
	void DoneWriting();

	Flag finalized = false;
	::grpc::Status status = ::grpc::Status::OK;
};

} // namespace client
} // namespace grpc
} // namespace dlg

#endif // __DLG_GRPC_CLIENT_FINAL_HEADER__
