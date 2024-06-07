#ifndef __DLG_GRPC_READER_HEADER__
#define __DLG_GRPC_READER_HEADER__

#include "operation.h"

#include "../basic.h"

namespace dlg
{
namespace grpc
{

template< class C, typename R >
struct ReaderBase : public Operation
{
	using Core = C;
	using Message = R;

	void Read( Message& ) {}
	bool Verify() const { return true; }
};

template< class C, typename R >
struct ReaderBase< C, Stream<R> > : public Operation
{
	using Core = C;
	using Message = R;

	bool Verify() const { return true; }
	void Read( R& );
};

template< typename Base >
struct Reader : public Base
{
	using typename Base::Core;
	using typename Base::Message;

	void Read()
	{
		return Base::Read(message);
	}

	void Done(bool) override;

	Message message;
};

} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_READER_HEADER__

