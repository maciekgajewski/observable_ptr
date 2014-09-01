#define BOOST_TEST_MODULE observable_ptr_tests
#define BOOST_TEST_MAIN 1

#include <observable_ptr/observable_ptr.hpp>

#include <boost/test/unit_test.hpp>


struct test_struct
{
	static int destructor_calls;

	int field;

	~test_struct() { destructor_calls++; }
};

int test_struct::destructor_calls = 0;

BOOST_AUTO_TEST_CASE(basic_tests)
{
	// default construction
	observable_ptr<test_struct> op;

	BOOST_REQUIRE_EQUAL(op, false);

	// assignent to object
	test_struct::destructor_calls = 0;
	op = new test_struct{7};
	BOOST_REQUIRE_EQUAL(op, true);
	BOOST_REQUIRE_EQUAL(op->field, 7);

	// assigment to nullptr
	BOOST_REQUIRE_EQUAL(test_struct::destructor_calls, 0);
	op = nullptr;
	BOOST_REQUIRE_EQUAL(test_struct::destructor_calls, 1);

	// destruction of object on own destruction
	{
		observable_ptr<test_struct> op2(new test_struct{42});
		BOOST_REQUIRE_EQUAL(op2, true);
		BOOST_REQUIRE_EQUAL(op2->field, 42);
	}
	BOOST_REQUIRE_EQUAL(test_struct::destructor_calls, 2);

}

BOOST_AUTO_TEST_CASE(ownership_transfer)
{
	test_struct::destructor_calls = 0;

	// ownership transfer via constructor
	observable_ptr<test_struct> op1 = new test_struct{8};
	observable_ptr<test_struct> op2(std::move(op1));
	observable_ptr<test_struct> op3;


	BOOST_REQUIRE_EQUAL(op1, false);
	BOOST_REQUIRE_EQUAL(op2, true);
	BOOST_REQUIRE_EQUAL(op3, false);
	BOOST_REQUIRE_EQUAL(op2->field, 8);

	// ownerhip transfer via assignent operator
	op3 = std::move(op2);

	BOOST_REQUIRE_EQUAL(op1, false);
	BOOST_REQUIRE_EQUAL(op2, false);
	BOOST_REQUIRE_EQUAL(op3, true);
	BOOST_REQUIRE_EQUAL(op3->field, 8);

	// ownership transfer via swap
	op1.swap(op3);

	BOOST_REQUIRE_EQUAL(op1, true);
	BOOST_REQUIRE_EQUAL(op2, false);
	BOOST_REQUIRE_EQUAL(op3, false);
	BOOST_REQUIRE_EQUAL(op1->field, 8);

	// ownership transfer via std::swap
	std::swap(op2, op1);
	BOOST_REQUIRE_EQUAL(op1, false);
	BOOST_REQUIRE_EQUAL(op2, true);
	BOOST_REQUIRE_EQUAL(op3, false);
	BOOST_REQUIRE_EQUAL(op2->field, 8);

	BOOST_REQUIRE_EQUAL(test_struct::destructor_calls, 0);
}

BOOST_AUTO_TEST_CASE(observers)
{
	observable_ptr<test_struct> op1 = new test_struct{12};
	BOOST_REQUIRE_EQUAL(op1, true);
	BOOST_REQUIRE_EQUAL(op1->field, 12);

	observer_ptr<test_struct> w1(op1); // initialized from observed

	BOOST_REQUIRE_EQUAL(op1, true);
	BOOST_REQUIRE_EQUAL(op1->field, 12);

	BOOST_REQUIRE_EQUAL(w1, true);
	BOOST_REQUIRE_EQUAL(w1->field, 12);

	observer_ptr<test_struct> w2 = w1; // initialized w1

	BOOST_REQUIRE_EQUAL(op1, true);
	BOOST_REQUIRE_EQUAL(op1->field, 12);

	BOOST_REQUIRE_EQUAL(w1, true);
	BOOST_REQUIRE_EQUAL(w1->field, 12);

	BOOST_REQUIRE_EQUAL(w2, true);
	BOOST_REQUIRE_EQUAL(w2->field, 12);

	op1 = nullptr;

	BOOST_REQUIRE_EQUAL(op1, false);
	BOOST_REQUIRE_EQUAL(w1, false);
	BOOST_REQUIRE_EQUAL(w2, false);
}

