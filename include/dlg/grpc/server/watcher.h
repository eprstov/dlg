#ifndef __DLG_GRPC_SERVER_WATCHER_HEADER__
#define __DLG_GRPC_SERVER_WATCHER_HEADER__

namespace dlg
{
namespace grpc
{
namespace server
{

template< class C >
struct Watcher : public Operation
{
	void Done(bool) override;
};

} // namespace server
} // namespace grpc
} // namespace dlg

#endif // !__DLG_GRPC_SERVER_WATCHER_HEADER__
