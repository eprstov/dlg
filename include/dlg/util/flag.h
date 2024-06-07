#ifndef __DLG_UTIL_FLAG_HEADER__
#define __DLG_UTIL_FLAG_HEADER__

namespace dlg
{

struct Flag
{
	Flag( bool val = false ) : value(val) {}

	bool Raise()
	{
		return Set(true);
	}

	bool Drop()
	{
		return Set(false);
	}

	bool Set( bool val )
	{
		bool res = value != val;
		value = val;

		return res;
	}

	operator bool() const
	{
		return value;
	}

	bool value;
};

} // namespace dlg

#endif // !__DLG_UTIL_FLAG_HEADER__
