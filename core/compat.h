#pragma once
#include <cstddef>

#if !defined(__cpp_size_t_suffix)
namespace detail {
	consteval std::size_t operator"" _uz(unsigned long long n) {
		return static_cast<std::size_t>(n);
	}
} // namespace detail
using detail::operator"" _uz;
#define uz _uz
#endif