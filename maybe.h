#ifndef _COMMONUTIL_MAYBE_H
#define _COMMONUTIL_MAYBE_H

#include <new>
#include <utility>
#include <stdexcept>
#include <type_traits>

#define CHECK_MAYBE() do { if (empty()) throw std::runtime_error("Maybe is Nothing"); } while (0) 

class Nothing
{
};

template <typename T>
class Maybe
{
public:
	Maybe()
		: m_hasData(false)
	{ }
	Maybe(Nothing)
		: m_hasData(false)
	{ }
	Maybe(const Maybe<T>& m)
		: m_hasData(m.m_hasData)
	{
		if (m_hasData)
			new (m_buffer)T(*m->getPtr());
	}
	Maybe(Maybe<T>&& m)
		: m_hasData(m.m_hasData)
	{
		if (m_hasData)
			new (m_buffer)T(std::move(*m->getPtr()));
	}
	Maybe(const T& t)
		: hasData(true)
	{
		new (m_buffer)T(t);
	}
	Maybe(T&& t)
		: hasData(true)
	{
		new (m_buffer)T(std::move(t));
	}
	~Maybe()
	{
		if (m_hasData)
			getPtr()->~T();
	}
	Maybe<T>& operator=(const Maybe<T>& other)
	{
		if (this == &other) return;
		if (m_hasData) getPtr()->~T();
		if (m_hasData = other.m_hasData)
			new (m_buffer)T(other);
		return *this;
	}
	Maybe<T>& operator=(Maybe<T>&& other)
	{
		if (this == &other) return;
		if (m_hasData) getPtr()->~T();
		if (m_hasData = other.m_hasData)
			new (m_buffer)T(std::move(other));
		return *this;
	}

	T& operator*() { CHECK_MAYBE(); return *getPtr(); }
	const T& operator*() const { CHECK_MAYBE(); return *getPtr(); }

	T* operator->() { CHECK_MAYBE(); return getPtr(); }
	const T* operator->() const { CHECK_MAYBE(); return getPtr(); }

	bool empty() const { return !m_hasData; }

	explicit operator bool() { return m_hasData; }

	T* ptr() { return m_hasData ? getPtr() : nullptr; }
	const T* ptr() const { return m_hasData ? getPtr() : nullptr; }

private:

	T* getPtr() { return reinterpret_cast<T*>(m_buffer); }
	const T* getPtr() const { return reinterpret_cast<const T*>(m_buffer); }

	char m_buffer[sizeof(T)];
	bool m_hasData;
};

template <typename T>
class Maybe<T&>
{
public:
	Maybe()
		: m_ptr(nullptr)
	{ }
	Maybe(Nothing)
		: m_ptr(nullptr)
	{ }
	Maybe(const Maybe<T&>& m)
		: m_ptr(m.m_ptr)
	{ }
	Maybe(const Maybe<T>& m)
		: m_ptr(m.ptr())
	{ }
	Maybe(T& t)
		: m_ptr(&t)
	{
	}
	Maybe<T&> operator=(Maybe<T&> other)
	{
		m_ptr = other.m_ptr;
	}

	T& operator*() { CHECK_MAYBE(); return *getPtr(); }
	const T& operator*() const { CHECK_MAYBE(); return *getPtr(); }

	T* operator->() { CHECK_MAYBE(); return getPtr(); }
	const T* operator->() const { CHECK_MAYBE(); return getPtr(); }

	bool empty() const { return m_ptr == nullptr; }

	explicit operator bool() { return m_ptr != nullptr; }

	T* ptr() { return getPtr(); }
	const T* ptr() const { return getPtr(); }
private:
	T* getPtr() { return m_ptr; }
	const T* getPtr() const { return m_ptr; }

	T* m_ptr;
};

#endif
