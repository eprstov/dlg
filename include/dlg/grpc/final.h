#ifndef __DLG_GRPC_FINAL_HEADER__
#define __DLG_GRPC_FINAL_HEADER__

#include "operation.h"

namespace dlg
{
namespace grpc
{

template< typename C >
struct FinalizerBase : public Operation
{
	void Finish() {}
	void DoneReading();
	void DoneWriting();

	void Done(bool) override;
};

} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_FINAL_HEADER__
