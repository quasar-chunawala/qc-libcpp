// switch.h
#include <cstdint>
#include <numeric>
#include <utility>
#include <variant>

namespace dev {
namespace tools::v1 {

template<std::size_t size, typename Func>
struct switcher
{
    Func func_;
    using result_t = decltype(func_(std::integral_constant<std::size_t, 0>()));
    constexpr switcher(std::integral_constant<std::size_t, size>, Func func)
      : func_{ func }
    {
    }

    constexpr result_t operator()(std::size_t i) const
    {
        using case_ = result_t (*)(Func);
        return [&]<std::size_t... idxs>(std::index_sequence<idxs...>) {
            // an array of anonymous functions
            constexpr case_ cases[]{ [](Func func) {
                return func(std::integral_constant<std::size_t, idxs>());
            }... };
            return cases[i](func_);
        }(std::make_index_sequence<size>{});
    }
};
}
}

namespace dev {
namespace tools::v2 {
template<std::size_t size, typename Func>
struct switcher
{
    Func func_;
    using res_t = decltype(func_(std::integral_constant<std::size_t, 0>()));
    switcher(std::integral_constant<std::size_t, size>, Func func)
      : func_{ func }
    {
    }

    constexpr res_t operator()(std::size_t i)
    {
        return [&]<std::size_t... idxs>(std::index_sequence<idxs...>) {
            res_t result;
            (((i == idxs)
                ? (result = func_(std::integral_constant<std::size_t, idxs>()), 0)
                : 0),
             ...);
            return result;
        }(std::make_index_sequence<size>());
    }
};
}
}

// Consider that we have variants v1, v2, v3 each having 2, 3, and 5
// alternatives respectively. Then, we have the following:
// 0,0,0 maps to -> Case 0
// 0,0,1 maps to -> Case 1
// ...
// 0,1,0 maps to -> Case 5
// 0,1,1 maps to -> Case 6
// ...
// 0,2,4 maps to -> Case 14
// 1,0,0 maps to -> Case 15
// ...
// 1,2,4 maps to -> Case 29
// In general, (a,b,c) maps to -> 15a + 5b + c.

// math::coeffs returns the coefficients array [15, 5, 1].
namespace dev::tools {
template<std::size_t... dims>
struct math_array
{
    using multi_idx = std::array<std::size_t, sizeof...(dims)>;
    static constexpr multi_idx coeffs = []() {
        std::array dims_a = { dims... };
        std::array result = dims_a;
        for (std::size_t running = 1, i = dims_a.size(); i > 0; --i) {
            result[i - 1] = running;
            running *= dims_a[i - 1];
        }
        return result;
    }();

    static constexpr multi_idx to_multi_idx(std::size_t linear_idx)
    {
        std::array dims_a = { dims... };
        multi_idx result;
        multi_idx coeffs_ = coeffs();

        for (std::size_t i = 0; i < coeffs.size(); ++i) {
            result[i] = linear_idx / coeffs_[i];
            linear_idx -= result[i];
        }
        return result;
    };

    static constexpr std::size_t to_linear_idx(multi_idx m_idx)
    {
        std::size_t result{ 0uz };
        multi_idx coeffs_ = coeffs();
        result = std::inner_product(coeffs_.begin(), coeffs_.end(), m_idx.begin(), 0u);
        return result;
    }

    template<std::size_t linear_idx>
    static constexpr auto to_multi_idx_sequence()
    {
        return [&]<std::size_t... idxs>(std::index_sequence<idxs...>) {
            constexpr auto arr = to_multi_idx(linear_idx);
            return std::index_sequence<arr[idxs]...>();
        }(std::make_index_sequence<sizeof...(dims)>());
    }
};
};

std::size_t
test(std::size_t init, std::size_t select)
{
    dev::tools::v2::switcher switch_{
        std::integral_constant<std::size_t, 10>{},
        [&]<std::size_t i>(std::integral_constant<std::size_t, i>) { return init + i; }
    };
    return switch_(select);
}

namespace dev {
template<typename Visitor, typename... Variants>
auto
visit(Visitor&& visitor, Variants&&... variants)
{
    using c_math_array = tools::math_array<std::variant_size_v<Variants>...>;
    constexpr std::size_t num_cases = (std::variant_size_v<Variants> * ...);
    tools::v2::switcher switcher{
        std::integral_constant<std::size_t, num_cases>{},
        [&]<std::size_t case_>(std::integral_constant<std::size_t, case_>) {
            constexpr auto seq =
              typename c_math_array::template to_multi_idx_sequence<case_>();
            return [&]<std::size_t... idxs>(std::index_sequence<idxs...>) {
                return std::forward<Visitor>(visitor)(
                  std::get<idxs>(std::forward<Variants>(variants).data)...);
            }(seq);
        }
    };

    auto idx = c_math_array::to_linear_idx({ variants.index... });
}
}