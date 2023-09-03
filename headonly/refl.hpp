#pragma once

#include <type_traits>

namespace refl
{
    namespace detail
    {
        [[noreturn]] inline void unreachable()
        {
#ifdef __GNUC__ // GCC, Clang, ICC
            __builtin_unreachable();
#elif defined(_MSC_VER) // msvc
            __assume(false);
#endif
        }

        struct universal_t {
            template <typename T>
            operator T()
            {
                unreachable();
            }
        };

        template <typename T>
        using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

        //
        // range type -> T::begin() && T::end()
        //

        template <typename T, typename = void>
        struct is_range : std::false_type {
        };
        template <typename T>
        struct is_range<T, std::void_t<decltype(T().begin()), decltype(T().end())>>
            : std::true_type {
        };
        template <typename T>
        constexpr bool is_range_v = detail::is_range<T>::value;

        //
        // check string view type -> T::c_str() && T::size()
        //

        template <typename T, typename = void>
        struct is_string_view : std::false_type {
        };
        template <typename T>
        struct is_string_view<T, std::void_t<decltype(T().c_str()), decltype(T().size())>>
            : std::true_type {
        };
        template <typename T>
        constexpr bool is_string_view_v = detail::is_string_view<T>::value;

        //
        // check initializer to construct struct
        //

        template <typename T, typename Initializer, typename = void, typename... Args>
        struct is_initializer : std::false_type {
        };
        template <typename T, typename Initializer, typename... Args>
        struct is_initializer<T,
                              Initializer,
                              std::void_t<decltype(T{{Args{}}..., {Initializer{}}})>,
                              Args...> : std::true_type {
        };
        template <typename T, typename Initializer, typename... Args>
        constexpr bool is_initializer_v =
            detail::is_initializer<T, Initializer, void, Args...>::value;

        struct adl {

            template <typename T, typename... Args>
            static constexpr std::size_t counts_impl()
            {
                if constexpr (detail::is_initializer_v<T, detail::universal_t, Args...>) {
                    return counts_impl<T, Args..., detail::universal_t>();
                }
                else {
                    return sizeof...(Args);
                }
            }

            template <size_t Count, typename T, typename Visitor>
            static constexpr decltype(auto) visits_impl(const T &obj, Visitor &&visitor)
            {
                static_assert(Count <= 16, "Too many structure members.");

                if constexpr (Count == 0) {
                    return visitor();
                }
                else if constexpr (Count == 1) {
                    auto &&[p1] = obj;
                    return visitor(p1);
                }
                else if constexpr (Count == 2) {
                    auto &&[p1, p2] = obj;
                    return visitor(p1, p2);
                }
                else if constexpr (Count == 3) {
                    auto &&[p1, p2, p3] = obj;
                    return visitor(p1, p2, p3);
                }
                else if constexpr (Count == 4) {
                    auto &&[p1, p2, p3, p4] = obj;
                    return visitor(p1, p2, p3, p4);
                }
                else if constexpr (Count == 5) {
                    auto &&[p1, p2, p3, p4, p5] = obj;
                    return visitor(p1, p2, p3, p4, p5);
                }
                else if constexpr (Count == 6) {
                    auto &&[p1, p2, p3, p4, p5, p6] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6);
                }
                else if constexpr (Count == 7) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7);
                }
                else if constexpr (Count == 8) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8);
                }
                else if constexpr (Count == 9) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9);
                }
                else if constexpr (Count == 10) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
                }
                else if constexpr (Count == 11) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
                }
                else if constexpr (Count == 12) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
                }
                else if constexpr (Count == 13) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
                }
                else if constexpr (Count == 14) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = obj;
                    return visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
                }
                else if constexpr (Count == 15) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] = obj;
                    return visitor(p1,
                                   p2,
                                   p3,
                                   p4,
                                   p5,
                                   p6,
                                   p7,
                                   p8,
                                   p9,
                                   p10,
                                   p11,
                                   p12,
                                   p13,
                                   p14,
                                   p15);
                }
                else if constexpr (Count == 16) {
                    auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16] =
                        obj;
                    return visitor(p1,
                                   p2,
                                   p3,
                                   p4,
                                   p5,
                                   p6,
                                   p7,
                                   p8,
                                   p9,
                                   p10,
                                   p11,
                                   p12,
                                   p13,
                                   p14,
                                   p15,
                                   p16);
                }
            }

        };

    } // namespace detail

    //
    // get struct member counts.
    //

    template <typename T>
    constexpr std::size_t counts()
    {
        using type_t = detail::remove_cvref_t<T>;
        return detail::adl::counts_impl<type_t>();
    }

    template <typename T>
    constexpr std::size_t counts(T &&obj)
    {
        return counts<decltype(obj)>();
    }

    //
    // visit members
    //

    template <typename T, typename Visitor>
    constexpr decltype(auto) visits(T &&obj, Visitor &&visitor)
    {
        using type_t = detail::remove_cvref_t<decltype(obj)>;
        return detail::adl::visits_impl<counts<type_t>(), T, Visitor>(
            std::forward<T>(obj),
            std::forward<Visitor>(visitor));
    }

    //
    // foreach members
    //

    template <typename T, typename Func>
    void foreach(T &&obj, Func &&func)
    {
        visits(
            obj,
            [&](auto &&...items) constexpr { (func(items), ...); });
    }

} // namespace refl
