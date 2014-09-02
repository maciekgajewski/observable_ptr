#pragma once

#include <observable_ptr/observable_ptr.hpp>
#include <memory>
#include <vector>


struct test_struct
{
	int counter;
	std::int64_t a;
	std::int64_t b;
	std::int64_t c;
	std::int64_t d;
	std::int64_t e;
};

struct base_create
{
	base_create(std::size_t size)
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

	void null()
	{
		raw_.push_back(new test_struct);
	}

	std::vector<test_struct*> raw_;
};

struct create_observable : public base_create
{
	create_observable(std::size_t size) : base_create(size)
	{
		observable_.reserve(size);
	}

	void action()
	{
		observable_.emplace_back(new test_struct);
	}

	std::vector<observable_ptr<test_struct>> observable_;

};

struct create_shared : public base_create
{
	create_shared(std::size_t size) : base_create(size)
	{
		shared_.reserve(size);
	}

	void action()
	{
		shared_.emplace_back(std::make_shared<test_struct>());
	}

	std::vector<std::shared_ptr<test_struct>> shared_;
};
