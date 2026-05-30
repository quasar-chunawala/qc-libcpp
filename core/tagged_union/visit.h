#include <array>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <variant>

#include <core/tagged_union/utilities.h>

// https://godbolt.org/z/f4bTvG5cd

#include <array>
#include <format>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <numeric>
#include <print>
#include <utility>
#include <variant>

namespace dev {
	namespace tools {

		template <typename T, size_t Dim, size_t... MoreDims>
		struct poly_array;

		// Base case - a 1d vector
		template <typename T, size_t Dim> struct poly_array<T, Dim> {
			std::array<T, Dim> func_;

			constexpr T &operator[](size_t i) { return func_[i]; }

			constexpr const T &operator[](size_t i) const {
				return func_[i];
			}

			constexpr poly_array(std::initializer_list<T> rng)
			    : poly_array() {
				auto it{rng.begin()};
				for (size_t i{0}; i < rng.size(); ++i) {
					func_[i] = *it;
					++it;
				}
			}

			constexpr poly_array &operator=(std::initializer_list<T> rng) {
				poly_array(rng).swap(*this);
				return *this;
			}

			constexpr poly_array &swap(poly_array<T, Dim> &other) {
				std::swap(func_, other.func_);
				return *this;
			}

			constexpr poly_array() : func_{} {}
		};

		// Generalization - a (sizeof...(MoreDims) + 1)d tensor
		template <typename T, size_t Dim, size_t... MoreDims>
		struct poly_array {
			std::array<poly_array<T, MoreDims...>, Dim> func_;

			constexpr poly_array<T, MoreDims...> &operator[](size_t i) {
				return func_[i];
			}

			constexpr const poly_array<T, MoreDims...> &
			operator[](size_t i) const {
				return func_[i];
			}

			template <typename U>
			constexpr poly_array(
			    std::initializer_list<std::initializer_list<U>> rng)
			    : poly_array{} {
				auto it{rng.begin()};
				for (size_t i{0}; i < rng.size(); ++i) {
					func_[i] = *it;
					++it;
				}
			}

			constexpr poly_array &
			swap(poly_array<T, Dim, MoreDims...> &other) {
				std::swap(func_, other.func_);
				return *this;
			}

			template <typename U>
			constexpr poly_array &operator=(
			    std::initializer_list<std::initializer_list<U>> rng) {
				poly_array(rng).swap(*this);
				return *this;
			}

			constexpr poly_array() : func_{} {}
		};

	}; // namespace tools

} // namespace dev

namespace dev::tools {
	template <std::size_t... Dimensions>
	constexpr auto build_coeffs_array() {
		constexpr std::array<std::size_t, sizeof...(Dimensions)>
		    dimensions{Dimensions...};
		constexpr std::size_t coeffs_size = sizeof...(Dimensions);
		std::array<std::size_t, coeffs_size> coeffs = {};

		for (std::size_t i{0}; i < coeffs_size; ++i)
			coeffs[i] = 1;

		for (std::size_t i{0}; i < coeffs_size - 1; ++i) {
			// In step i, we need to populate all coeffs[j], j <= i
			for (std::size_t j{0}; j <= i; ++j) {
				coeffs[j] *= dimensions[i + 1];
			}
		}
		return coeffs;
	}
	/*
	coeff[0] = v1size * ... * v[n-1] size
	coeff[1] = v2size * .... *v[n-1] size
	...
	coeff[n-2] = v[n-1] size
	coeff[n-1] = 1
	*/
	template <std::size_t... Dimensions>
	std::size_t constexpr to_index(auto... indices) {
		constexpr std::array<std::size_t, sizeof...(Dimensions)>
		    dimensions{Dimensions...};
		std::size_t coeffs_size = sizeof...(Dimensions);
		auto coeffs = build_coeffs_array<Dimensions...>();
		// std::println("{}", coeffs);
		const std::array<size_t, sizeof...(indices)> indices_arr{
		    indices...};
		// std::println("{}", indices_arr);
		return std::inner_product(
		    coeffs.begin(), coeffs.end(), indices_arr.begin(), 0);
	}

	/*
	Suppose we have the dimensions <3, 5, 2> and the coordinates (1, 3, 1).
	(1, 3, 1) maps to the linear index 17.
	*/

	template <typename Func, typename... Ts>
	constexpr auto for_each(Func &&func, std::tuple<Ts...> tup) {
		return [&]<size_t... Indices>(std::index_sequence<Indices...>) {
			return std::make_tuple(
			    std::forward<Func>(func)(std::get<Indices>(tup))...);
		}(std::make_index_sequence<sizeof...(Ts)>());
	}

	template <typename Func, typename T, size_t N>
	constexpr auto for_each(Func &&func, std::array<T, N> arr) {
		using result_t = decltype(func(T()));
		std::array<result_t, N> results{};
		return [&]<size_t... Indices>(std::index_sequence<Indices...>) {
			((results[Indices] = func(arr[Indices])), ...);
			return results;
		}(std::make_index_sequence<N>());
	}

	template <size_t N>
	constexpr auto build_coords_array(std::array<size_t, N> coeffs,
	                                  size_t &initState) {
		size_t state = initState;
		std::array<size_t, N> coords{};
		for (size_t i{0}; i < N; ++i) {
			coords[i] = static_cast<size_t>(state / coeffs[i]);
			state -= coords[i] * coeffs[i];
		}
		return coords;
	}

	template <size_t... Dimensions>
	decltype(auto) constexpr from_index(std::size_t n) {
		constexpr std::size_t coords_arr_size = sizeof...(Dimensions);
		constexpr auto coeffs = build_coeffs_array<Dimensions...>();
		std::array<size_t, coords_arr_size> coords =
		    build_coords_array(coeffs, n);
		return coords;
	}
} // namespace dev::tools

// Simple case of 2 variants
namespace dev {

	namespace example {

		struct Dummy {};
		template <typename T>
		using Wrapper = std::conditional_t<std::is_void_v<T>, Dummy, T>;

		template <typename Visitor, typename Variant0, typename Variant1>
		decltype(auto) visit(Visitor &&visitor, Variant0 v0, Variant1 v1) {
			constexpr std::array<std::size_t, 2> dimensions = {
			    std::variant_size_v<Variant0>,
			    std::variant_size_v<Variant1>};
			constexpr std::size_t vtable_size =
			    (std::variant_size_v<Variant0> *
			     std::variant_size_v<Variant1>);
			using cases_t = std::string (*)(Visitor, Variant0, Variant1);
			static constexpr auto vtable{
			    []<size_t... Indices>(std::index_sequence<Indices...>) {
				    return std::array<cases_t, 4>{
				        [](Visitor vis, Variant0 v0, Variant1 v1) {
					        constexpr auto multi_idx =
					            dev::tools::from_index<2, 2>(Indices);
					        return vis(std::get<multi_idx[0]>(v0),
					                   std::get<multi_idx[1]>(v1));
				        }...};
			    }(std::make_index_sequence<vtable_size>())};

			return vtable[dev::tools::to_index<2, 2>(
			    v0.index(), v1.index())](visitor, v0, v1);
		}
	}; // namespace example
} // namespace dev

template <typename... Callables> struct Visitor : Callables... {
	using Callables::operator()...;
};

void test_single_dispatch() {
	std::variant<int, float, double> v{0};
}

void test_double_dispatch() {
	std::variant<int, float> v1{0};
	std::variant<int, float> v2{3.14f};

	auto result = dev::example::visit(
	    Visitor{
	        [](int, int) -> std::string { return "(int, int)"; },
	        [](int, float) -> std::string { return "(int, float)"; },
	        [](float, int) -> std::string { return "(float, int)"; },
	        [](float, float) -> std::string { return "(float, float)"; },
	    },
	    v1,
	    v2);
	std::cout << "result = " << result << "\n";
}

namespace dev {
	template <typename Visitor, typename... Variants>
	decltype(auto) visit(Visitor &&visitor, Variants &&...vs) {
		constexpr std::size_t vtable_size =
		    (std::variant_size_v<std::remove_cvref_t<Variants>> * ...);

		// Each entry in the vtable should have the shape [](Visitor
		// visitor, Variants... vs){}
		using result_t = decltype(visitor(std::get<0>(vs)...));
		using cases_t = result_t (*)(Visitor, Variants...);

		static constexpr auto vtable{[]<size_t... Indices>(
		                                 std::index_sequence<Indices...>) {
			constexpr std::array<std::size_t, sizeof...(Variants)>
			    dimensions = {
			        std::variant_size_v<std::remove_cvref_t<Variants>>...};
			constexpr std::size_t vtable_size =
			    (std::variant_size_v<std::remove_cvref_t<Variants>> * ...);
			return std::array<cases_t, vtable_size>{[](auto vis,
			                                           auto... vs)
			                                            -> result_t {
				constexpr auto multi_idx = dev::tools::from_index<
				    std::variant_size_v<std::remove_cvref_t<Variants>>...>(
				    Indices);
				return [&]<size_t... Is>(std::index_sequence<Is...>) {
					return vis((static_cast<std::variant_alternative_t<
					                Is,
					                std::remove_cvref_t<Variants>>>(
					    std::get<multi_idx[Is]>(vs)))...);
				}(std::make_index_sequence<sizeof...(Variants)>());
			}...};
		}(std::make_index_sequence<vtable_size>())};

		auto i = dev::tools::to_index<
		    std::variant_size_v<std::remove_cvref_t<Variants>>...>(
		    vs.index()...);
		return vtable[i](visitor, vs...);
	}
} // namespace dev

void test_multiple_dispatch() {
	std::variant<int, float, double> v1{0};
	std::variant<int, float, double> v2{3.14f};
	// std::variant<char, int, long, float, double> v3{'H'};

	auto result = dev::visit(
	    Visitor{[](int, int) -> std::string { return "(int, int)"; },
	            [](int, float) -> std::string { return "(int, float)"; },
	            [](int, double) -> std::string { return "(int, double)"; },
	            [](float, int) -> std::string { return "(float, int)"; }},
	    v1,
	    v2);

	std::cout << "result = " << result << "\n";
}
