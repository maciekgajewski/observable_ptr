#ifndef __OBSERVABLE_PTR_OBSERVABLE_PTR_HPP__
#define __OBSERVABLE_PTR_OBSERVABLE_PTR_HPP__

#ifdef __gcc__
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#include <utility>
#include <cassert>

template<typename T> class observer_ptr;
template<typename T> class observable_ptr;

namespace detail
{
template<typename T>
class node
{
public:

	const T* operator->() const { assert(data_); return data_; }
	T* operator->() { assert(data_); return data_; }
	operator bool() const { return data_ != nullptr; }

protected:

	template<typename U> friend class observer_ptr;
	template<typename U> friend class observable_ptr;

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
		for(auto n = this->next_; n; n = n->next_)
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

	// Exposed for debugging
	std::size_t observer_count() const
	{
		std::size_t c = 0;
		for(auto n = this->next_; n; n = n->next_)
		{
			++c;
		}
		return c;
	}

private:

	template<typename U> friend class observer_ptr;

	void reset();
};

template<typename T, typename... Args>
observable_ptr<T> make_observable(Args&& ... aa)
{
	return observable_ptr<T>(new T(std::forward<Args>(aa)...));
}

template<typename T>
class observer_ptr : public detail::node<T>
{
	using observer_t = observer_ptr<T>;
	using node_t = detail::node<T>;
public:
	observer_ptr()
	{
	}

	observer_ptr(const observable_ptr<T>& o) : observer_ptr()
	{
		attach(o);
	}

	observer_ptr(const observer_ptr<T>& o) : observer_ptr()
	{
		attach(o);
	}

	~observer_ptr()
	{
		detach();
	}

	observer_ptr<T>& operator=(const observable_ptr<T>& o)
	{
		attach(o);
		return *this;
	}

	observer_ptr<T>& operator=(const observer_ptr<T>& o)
	{
		attach(o);
		return *this;
	}

private:

	template<typename W>
	void attach(W& o)
	{
		this->data_ = o.data_;
		if (o.data_)
		{
			this->next_ = o.next_;
			o.next_ = this;
			this->prev_ = (node_t*)&o;

			assert(this->prev_->next_ == this);
			if (this->next_)
			{
				this->next()->prev_ = this;
			}
		}
	}

	constexpr observer_ptr<T>* next() { return static_cast<observer_ptr<T>*>(this->next_); }

	void detach()
	{
		if (this->data_)
		{
			if (this->prev_)
			{
				assert(this->prev_->next_ == this);

				this->prev_->next_ = this->next_;
				if (this->next_)
				{
					assert(this->next()->prev_ == this);
					this->next()->prev_ = this->prev_;
				}
			}
			else
			{
				assert(this->next_ == nullptr && "if prev_ is null, nexT_ must be null too");
			}
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
	this->traverse_and_set(nullptr);
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
