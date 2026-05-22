#include <cassert>
#include <string>
#include <vector>

#include "v2/variant.h"

// ------------------ variant(T x) ctor tests ----------------

void test_default_construct() {
	dev::variant<int, float, double> v;
	assert(dev::get<0>(v) == 0);
}

void test_explicit_int_construct() {
	dev::variant<int, float, double> v(42);
	assert(dev::get<int>(v) == 42);
}

void test_explicit_float_construct() {
	dev::variant<int, float, double> v(3.14f);
	assert(dev::get<float>(v) == 3.14f);
}

void test_explicit_string_construct() {
	dev::variant<int, std::string, double> v(std::string("hello"));
	assert(dev::get<std::string>(v) == "hello");
}

// -------------------- copy constructor tests ------------------

void test_copy_int() {
	dev::variant<int, float, double> a(7);
	dev::variant<int, float, double> b(a);
	assert(dev::get<int>(b) == 7);
}

void test_copy_string() {
	dev::variant<int, std::string> a(std::string("copy me"));
	dev::variant<int, std::string> b(a);
	assert(dev::get<std::string>(b) == "copy me");
	dev::get<std::string>(b) += " mutated";
	assert(dev::get<std::string>(a) == "copy me");
}

// ------------------- operator=(T x) tests -------------------

void test_converting_assignment() {
	dev::variant<int, std::string, float> v;
	assert(dev::get<int>(v) == 0);
	v = 42;
	assert(dev::get<int>(v) == 42);
	v = std::string("hello");
	assert(dev::get<std::string>(v) == "hello");
	v = 3.14159f;
	assert(dev::get<float>(v) == 3.14159f);
}

// ------------------- swap tests -------------------

void test_swap_different_alternatives() {
	dev::variant<int, std::vector<int>> v1{
	    2
    },
	    v2{std::vector{1, 2, 3, 4}};
	assert(dev::get<0>(v1) == 2);
	assert(dev::get<1>(v2) == (std::vector<int>{1, 2, 3, 4}));
	v1.swap(v2);
	assert(dev::get<1>(v1) == (std::vector<int>{1, 2, 3, 4}));
	assert(dev::get<0>(v2) == 2);
}

void test_swap_same_alternative() {
	dev::variant<int, std::string> v1{std::string("hello")},
	    v2{std::string("world")};
	assert(dev::get<std::string>(v1) == "hello");
	assert(dev::get<std::string>(v2) == "world");
	v1.swap(v2);
	assert(dev::get<std::string>(v1) == "world");
	assert(dev::get<std::string>(v2) == "hello");
}

// ------------------- copy assignment tests -------------------

void test_copy_assignment_same_idx() {
	dev::variant<std::string, float> v;
	assert(dev::get<std::string>(v) == "");
	dev::variant<std::string, float> w(std::string("hello"));
	v = w;
	assert(dev::get<0>(v) == "hello");
	assert(dev::get<0>(w) == "hello");
}

void test_copy_assignment_different_idx() {
	dev::variant<std::vector<int>, int> v1{
	    std::vector{1, 2, 3, 4, 5}
    },
	    v2{42};
	assert(v1.index() == 0);
	assert(v2.index() == 1);
	v1 = v2;
	assert(v1.index() == 1);
	assert(dev::get<1>(v1) == 42);
}

// ------------------- move assignment tests -------------------

void test_move_assignment_same_idx() {
	dev::variant<std::vector<int>, std::string> v;
	assert(dev::get<0>(v).empty());
	dev::variant<std::vector<int>, std::string> w{
	    std::vector{1, 2, 3, 4, 5}
    };
	v = std::move(w);
	assert(!dev::get<0>(v).empty());
	assert(dev::get<0>(v).size() == 5);
	for (size_t i{0}; i < 5; ++i)
		assert(dev::get<0>(v)[i] == static_cast<int>(i + 1));
	assert(dev::get<0>(w).empty());
}

void test_move_assignment_different_idx() {
	dev::variant<std::vector<int>, std::string> v{
	    std::vector{1, 2, 3, 4, 5}
    };
	assert(v.index() == 0);
	dev::variant<std::vector<int>, std::string> w{std::string("hello")};
	assert(w.index() == 1);
	v = std::move(w);
	assert(v.index() == 1);
	assert(dev::get<1>(v) == "hello");
	assert(w.index() == 1);
	assert(dev::get<1>(w).empty());
}