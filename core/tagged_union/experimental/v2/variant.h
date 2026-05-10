#include "../utilities.h"
#include <cstddef>
#include <type_traits>
#include <utility>

// variant definition using constexpr-time
// array of function pointers approach
namespace dev {

	template <typename... Ts> class variant;

	// Forward declaration
	// so the friend declaration inside variant resolves
	template <size_t I, typename... Ts>
	constexpr decltype(auto) get(const variant<Ts...> &v);

	template <typename T, typename... Ts>
	constexpr T &get(const variant<Ts...> &v);

	template <typename... Ts> class variant {
		using index_type = tools::uint_atleast_t<sizeof...(Ts)>;
		static constexpr size_t size = sizeof...(Ts);

		tools::union_<Ts...> m_data;
		index_type m_idx;

		/**
		 * @brief Helper routine that applies the @param func to
		 * the currently held alternative. Internally uses a table of
		 * function pointers generated at compile time.
		 *
		 * @tparam Func
		 * @param func
		 * @return decltype(auto)
		 */
		template <typename Func> decltype(auto) apply_helper(Func func) {
			auto wrapper = [this, &func](std::size_t idx) {
				constexpr auto seq =
				    std::make_index_sequence<sizeof...(Ts)>();
				return [&]<std::size_t... Indices>(
				           std::index_sequence<Indices...>) {
					using result_t =
					    decltype(func(tools::get_nth_type_t<0, Ts...>()));
					using case_ = result_t (*)(Func);
					constexpr case_ cases[] = {[](Func &func_) {
						return func_(
						    tools::get_nth_type_t<Indices, Ts...>());
					}...};
					return cases[idx](func);
				}(seq);
			};

			return wrapper(m_idx);
		}

	public:
		/**
		 * @brief: Constructs a variant holding the zero-initialized value
		 * of the first alternative.
		 */
		constexpr variant() {
			tools::construct_at<0>(m_data,
			                       tools::get_nth_type_t<0, Ts...>());
			m_idx = 0;
		}

		/**
		 * @brief Converting constructor. Constructs a variant holding the
		 * alternative of type @p T, from @p x.
		 */
		template <tools::exists<Ts...> T>
		constexpr explicit variant(T &&x) {
			tools::construct_at<tools::find_index_of_v<T, Ts...>>(
			    m_data, TOOLS_FWD(x));
			m_idx = tools::find_index_of_v<T, Ts...>;
		}

		/**
		 * @brief Copy constructor. Constructs a variant holding the same
		 * alternative as @p other, copy-constructed from the contained
		 * value.
		 */
		constexpr variant(const variant &other) {
			index_type idx = other.m_idx;
			constexpr auto idx_seq =
			    std::make_index_sequence<sizeof...(Ts)>();
			[&]<auto... Indices>(std::index_sequence<Indices...>) {
				(((idx == Indices)
				      ? (tools::construct_at<Indices>(
				             m_data, tools::get<Indices>(other.m_data)),
				         0)
				      : 0),
				 ...);
			}(idx_seq);
			m_idx = idx;
		}

		/**
		 * @brief Move constructor. Steals data from @p other.
		 * Constructs a variant holding the same
		 * alternative as @p other, move-constructed from
		 * the contained value.
		 */
		constexpr variant(variant &&other) noexcept {
			index_type idx = other.m_idx;
			constexpr auto idx_seq =
			    std::make_index_sequence<sizeof...(Ts)>();
			[&]<auto... Indices>(std::index_sequence<Indices...>) {
				(((idx == Indices)
				      ? (tools::construct_at<Indices>(
				             m_data,
				             tools::get<Indices>(std::move(other.m_data))),
				         0)
				      : 0),
				 ...);
			}(idx_seq);
			m_idx = idx;
		}

		/**
		 * @brief Swaps the contents of two variants.
		 *
		 * If both variants hold the same alternative (index() ==
		 * other.index()), the contained values are swapped directly via @c
		 * std::swap. Otherwise, the alternatives are exchanged by using a
		 * temporary.
		 */
		void swap(variant<Ts...> &other) noexcept {
			using std::swap;
			if (m_idx == other.m_idx) {
				// swap(get<m_idx>(*this), tools::get<m_idx>(other));
				return [&]<size_t... Idxs>(std::index_sequence<Idxs...>) {
					(((m_idx == Idxs)
					      ? (swap(get<Idxs>(*this), get<Idxs>(other)), 0)
					      : 0),
					 ...);
				}(std::make_index_sequence<sizeof...(Ts)>());
			} else {
				auto p = std::move(*this);
				std::construct_at(this, std::move(other));
				std::construct_at(&other, std::move(p));
			}
		}

		/**
		 * @brief Copy assignment operator.
		 */
		variant<Ts...> &operator=(const variant<Ts...> &other) {
			variant(other).swap(*this);
			return *this;
		}

		/**
		 * @brief Move assignment operator.
		 */
		variant<Ts...> &operator=(variant<Ts...> &&other) noexcept {
			variant(std::move(other)).swap(*this);
			return *this;
		}

		/**
		 * @brief Converting assignment operator. Assigns a value of type
		 * @p T to the variant.
		 */
		template <tools::exists<Ts...> T>
		constexpr variant<Ts...> &operator=(T &&element) {
			this->~variant();
			constexpr size_t i = tools::find_index_of_v<T, Ts...>;
			tools::construct_at<i>(m_data, TOOLS_FWD(element));
			m_idx = i;
			return *this;
		}

		/**
		 * @brief Destructor. Destroys the currently held alternative.
		 */
		constexpr ~variant() {
			auto wrapper = [this](std::size_t idx) {
				constexpr auto idx_seq =
				    std::make_index_sequence<sizeof...(Ts)>();
				return [&]<std::size_t... Indices>(
				           std::index_sequence<Indices...>) {
					using destructor_fn = void (*)(variant<Ts...> &);

					constexpr destructor_fn cases[]{[](variant<Ts...> &v) {
						tools::destroy_at<Indices>(v.m_data);
					}...};
					return cases[idx](*this);
				}(idx_seq);
			};

			wrapper(m_idx);
		}

		/**
		 * @brief Returns the zero-based index of the alternative currently
		 * held by the variant.
		 */
		constexpr std::size_t index() const noexcept { return m_idx; }

		template <size_t I, typename... Us>
		constexpr friend decltype(auto) get(const variant<Us...> &v);

		template <typename T, typename... Us>
		constexpr friend T &get(const variant<Us...> &v);

		template <size_t I, typename... Us>
		constexpr friend decltype(auto) get(variant<Us...> &v);

		template <typename T, typename... Us>
		constexpr friend T &get(variant<Us...> &v);
	};

	template <size_t I, typename... Ts>
	constexpr decltype(auto) get(const variant<Ts...> &v) {
		return tools::get<I>(v.m_data);
	}

	template <typename T, typename... Ts>
	constexpr T &get(const variant<Ts...> &v) {
		return tools::get<tools::find_index_of_v<T, Ts...>>(v.m_data);
	}

	template <size_t I, typename... Ts>
	constexpr decltype(auto) get(variant<Ts...> &v) {
		return tools::get<I>(v.m_data);
	}

	template <typename T, typename... Ts>
	constexpr T &get(variant<Ts...> &v) {
		return tools::get<tools::find_index_of_v<T, Ts...>>(v.m_data);
	}
} // namespace dev

#include <cassert>
#include <string>
#include <vector>

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
	for (auto i{0uz}; i < 5; ++i)
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