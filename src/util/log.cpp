#include <dlg/util/log.h>

#include <iostream>
#include <streambuf>

namespace dlg
{

#if !defined(TRACE) || TRACE == 0

namespace voidlog
{

class Buffer : public std::streambuf
{
public:
	int overflow(int c) { return c; }
};

Buffer buf;
std::ostream stream( &buf );

} // namespace voidlog

std::ostream& log = voidlog::stream;

#else

std::ostream& log = std::cout;

#endif

} // namespace dlg
