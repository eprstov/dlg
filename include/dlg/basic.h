#ifndef __DLG_BASIC_HEADER__
#define __DLG_BASIC_HEADER__

#include <functional>

namespace dlg
{
namespace trait
{

template< typename T > constexpr bool is_stream = false;
template< typename T > concept stream = is_stream<T>;
template< typename T > concept single = !is_stream<T>;

} // namespace trait

template< trait::single T > requires ( !std::is_same_v< T, void > ) struct Stream;

template< typename T > constexpr bool trait::is_stream< Stream<T> > = true;

namespace trait
{
namespace detail
{

template< typename T > struct Single
{
	using Type = T;
};

template< trait::single T > struct Single< Stream<T> >
{
	using Type = T;
};

} // namespace detail

template< typename T > using Single = typename detail::Single<T>::Type;

} // namespace trait

template< typename T = void > struct Destructible;

template<> struct Destructible<void>
{
	virtual ~Destructible() = default;
};

template< class T > struct Destructible : Destructible<void>, T
{
	using T::T;
	using T::operator=;
};

template< typename T > struct Socket
{
	struct Tag
	{
		Tag() = default;
		Tag( const T& ) {}
	};

	virtual void Attach( T& ) = 0;
	virtual void Detach( Tag = {} ) = 0;
};

template<> class Socket<void> {};

template< class T > struct Consumer
{
	virtual void operator<<(T) = 0;
};

template< class T > struct Consumer< Stream<T> > : Consumer<T>
{
	struct Tag {};

	virtual void Shut( Tag = Tag() ) {}
};

template<> class Consumer<void> {};
template<> class Socket< Consumer<void> > {};

template< class T > using Producer = Socket< Consumer<T> >;

namespace trait
{
namespace detail
{

template< typename T > struct ProducerTag
{
	using Type = typename Producer<T>::Tag;
};

template< typename T > struct ProducerTag< Consumer<T> >
{
	using Type = typename Producer<T>::Tag;
};

} // namespace detail

template< typename T > using ConsumerTag = typename Consumer< Stream< trait::Single<T> > >::Tag;

template< typename T > using ProducerTag = typename detail::ProducerTag<T>::Type;

} // namespace trait

template< typename T > class BasicProducer : public Producer<T>
{
public:
	using typename Producer<T>::Tag;

	void Attach( Consumer<T>& csmr ) override
	{
		assert( !consumer );
		consumer = &csmr;
	}

	void Detach( Tag = Tag() ) override
	{
		assert(consumer);
		consumer = nullptr;
	}

protected:
	Consumer<T>* consumer = nullptr;
};

template<> struct BasicProducer<void> : Producer<void>
{
};

namespace detail
{

template< typename T > struct DuplexBase
{
	void Attach( Consumer<T>& consumer )
	{
		return operator Producer<T>&().Attach(consumer);
	}

	void Detach( trait::ProducerTag<T> = {} )
	{
		return operator Producer<T>&().Detach();
	}

	virtual operator Consumer<T>&() = 0;
	virtual operator Producer<T>&() = 0;
};

} // namespace detail

template< typename T > struct Duplex : detail::DuplexBase<T>
{
	void operator<<( T item )
	{
		operator Consumer<T>&().operator<<( std::move(item) );
	}
};

template< typename T > struct Duplex< Stream<T> > : detail::DuplexBase< Stream<T> >
{
	void operator<<( T item )
	{
		operator Consumer< Stream<T> >&().operator<<( std::move(item) );
	}

	void Shut( trait::ConsumerTag<T> tag = {} )
	{
		return operator Consumer<T>&().Shut(tag);
	}
};

template<> struct Duplex<void> : Consumer<void>, Producer<void> {};

template<class T> class ConsumerWrap : public Consumer<T>
{
public:
	using Delegate = std::function< void(T) >;

	ConsumerWrap( Delegate dlgt ) : delegate( std::move(dlgt) ) {}
	void operator<<( T t ) override
	{
		delegate( std::move(t) );
	}

private:
	Delegate delegate;
};

} // namespace dlg

#endif // __DLG_BASIC_HEADER__
