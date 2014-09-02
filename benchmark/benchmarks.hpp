#pragma once

#include <observable_ptr/observable_ptr.hpp>
#include <memory>
#include <vector>


struct test_struct
{
	std::size_t counter;
	std::int64_t a;
	std::int64_t b;
	std::int64_t c;
	std::int64_t d;
	std::int64_t e;
};

struct base_create
{
	base_create(std::size_t size)
		: size_(size)
	{
		raw_.reserve(size);
	}

	~base_create()
	{
		for(test_struct* s : raw_)
		{
			delete s;
		}
		raw_.clear();
	}

	void null(std::size_t)
	{
		raw_.push_back(new test_struct);
	}

	std::vector<test_struct*> raw_;
	std::size_t size_;
};

struct create_observable : public base_create
{
	create_observable(std::size_t size) : base_create(size)
	{
		observable_.reserve(size);
	}

	void action(std::size_t)
	{
		observable_.emplace_back(new test_struct);
	}

	bool verify()
	{
		return observable_.size() == size_;
	}

	std::vector<observable_ptr<test_struct>> observable_;

};

struct create_shared : public base_create
{
	create_shared(std::size_t size) : base_create(size)
	{
		shared_.reserve(size);
	}

	void action(std::size_t)
	{
		shared_.emplace_back(std::make_shared<test_struct>());
	}

	bool verify()
	{
		return shared_.size() == size_;
	}

	std::vector<std::shared_ptr<test_struct>> shared_;
};

struct base_read
{
	base_read(std::size_t size) : size_(size) {}

	void null(std::size_t i)
	{
		null_total_ += i;
	}

	bool verify()
	{
		return total_ == size_ * (size_-1) / 2;
	}


	std::size_t null_total_ = 0;
	std::size_t total_ = 0;
	std::size_t size_;
};

struct read_shared : public base_read
{
	read_shared(std::size_t size) : base_read(size)
	{
		for(std::size_t i = 0; i < size; i++)
		{
			shared_.emplace_back(std::make_shared<test_struct>());
			shared_.back()->counter = i;
		}
	}

	void action(std::size_t i)
	{
		total_ += shared_[i]->counter;
	}

	std::vector<std::shared_ptr<test_struct>> shared_;
};

struct read_observable : public base_read
{
	read_observable(std::size_t size) : base_read(size)
	{
		for(std::size_t i = 0; i < size; i++)
		{
			observable_.emplace_back(make_observable<test_struct>());
			observable_.back()->counter = i;
		}
	}

	void action(std::size_t i)
	{
		total_ += observable_[i]->counter;
	}

	std::vector<observable_ptr<test_struct>> observable_;
};
