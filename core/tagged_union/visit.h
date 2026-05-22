#include <array>
#include <initializer_list>
#include <memory>
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

	template <size_t... Indices> struct select_element {
		template <typename Callable>
		auto operator()(Callable callable, auto... vs) {
			return callable(std::get<Indices>(vs)...);
		}
	};

	template <typename Visitor, typename... Variants>
	decltype(auto) visit(Visitor &&visitor, Variants &&...vs) {
		// using cases_t = decltype();
	}

} // namespace dev
