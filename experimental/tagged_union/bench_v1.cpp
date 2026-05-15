#include <cstddef>
#include <cstdint>
#include <string>

#include <vector>

#include "v1/variant.h"

void test_variant_20_types() {
	// holds alternative 10: long double
	dev::variant<std::int8_t,
	             std::int16_t,
	             std::int32_t,
	             std::int64_t,
	             std::uint8_t,
	             std::uint16_t,
	             std::uint32_t,
	             std::uint64_t,
	             float,
	             double,
	             long double,
	             char,
	             unsigned char,
	             signed char,
	             wchar_t,
	             bool,
	             std::string,
	             std::wstring,
	             std::vector<int>,
	             std::vector<double>>
	    v(1.414L);
	(void)v;
}

int main() {
	for (size_t i{0uz}; i < 10'000'000; ++i)
		test_variant_20_types();
}