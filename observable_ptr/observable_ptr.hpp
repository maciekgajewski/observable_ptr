#ifndef __OBSERVABLE_PTR_OBSERVABLE_PTR_HPP__
#define __OBSERVABLE_PTR_OBSERVABLE_PTR_HPP__

#ifdef __gcc__
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#include <utility>

template<typename T> class observer_ptr;

namespace detail
{
template<typename T>
class node
{
public:

	const T* operator->() const { return data_; }
	T* operator->() { return data_; }
	operator bool() const { return data_ != nullptr; }

protected:

	node() = default;
	node(const node<T>&) = delete;

	node(T* data)
		: data_(data)
	{
	}

	void swap(node& o)
	{
		std::swap(data_, o.data_);
		std::swap(next_, o.next_);
	}

	void traverse_and_set(T* data)
	{
		for(auto n = next_; n; n = n->next_)
		{
			n->data_ = nullptr;
		}
	}

	mutable node<T>* next_ = nullptr;
	T* data_ = nullptr;
};
}

template<typename T>
class observable_ptr : public detail::node<T>
{
	using observer_t = observer_ptr<T>;
	using node_t = detail::node<T>;

public:

	observable_ptr() = default;
	observable_ptr(const observable_ptr<T>&) = delete;
	observable_ptr(observable_ptr<T>&& o) NOEXCEPT
	{
		swap(o);
	}
	observable_ptr(T* obj) NOEXCEPT
		: node_t(obj)
	{
	}
	~observable_ptr()
	{
		reset();
	}


	observable_ptr<T>& operator=(const observable_ptr<T>&) = delete;

	observable_ptr<T>& operator=(observable_ptr<T>&& o)
	{
		swap(o);
		return *this;
	}

	observable_ptr<T>& operator=(T* obj)
	{
		reset();
		node_t::data_ = obj;
		return *this;
	}

	void swap(observable_ptr<T>& o)
	{
		node_t::swap(o);
	}

private:
	template<typename U> friend class observer_ptr;

	void reset();
};

template<typename T>
class observer_ptr : public detail::node<T>
{
	using observer_t = observer_ptr<T>;
	using node_t = detail::node<T>;
public:
	observer_ptr()
	{
	}

	observer_ptr(const observable_ptr<T>& o)
	{
		attach(o);
	}

	observer_ptr(const observer_ptr<T>& o)
	{
		attach(o);
	}

	observer_ptr<T>& operator=(const observable_ptr<T>& o)
	{
		attach(o);
	}

	observer_ptr<T>& operator=(const observer_ptr<T>& o)
	{
		attach(o);
	}

private:

	template<typename W>
	void attach(W& o)
	{
		this->data_ = o.data_;
		if (o.data_)
		{
			o.next_ = this;
			this->prev_ = (node_t*)&o;
		}
	}

	template<typename U> friend class observable_ptr;

	mutable node_t* prev_ = nullptr;
};

template<typename T>
void observable_ptr<T>::reset()
{
	if (node_t::data_)
	{
		delete node_t::data_;
		node_t::data_ = nullptr;
	}
	node_t::traverse_and_set(nullptr);
	node_t::next_ = nullptr;
}

// std overloads
namespace std
{

template<typename T>
void swap(observable_ptr<T>& a, observable_ptr<T>& b)
{
	a.swap(b);
}

}
#endif
