#pragma once

#include <type_traits>

namespace REFL
{
namespace DETAIL
{

//
// max members
//

constexpr size_t MaxClassMembers = 16;

//
// type system
//

typedef enum class _TYPE_ID {

    UNKNOWN = 0,

    BYTE = 1,
    WORD,
    DWORD,
    QWORD,
    FLOAT,
    DOUBLE,

    UNICODE_STRING = 64,

    STRING = 128,
    WSTRING,
    ARRAY,
    MAP,

    CLASS = 255,

} TYPE_ID,
    *PTYPE_ID;

//
// remove const, volatile, reference
//

template <typename _Ty>
using RemoveCVRType = std::remove_cv_t<std::remove_reference_t<_Ty>>;

//
// string type -> _Ty::length() && _Ty::data() && _Ty::value_type is char or wchar_t
//

template <typename _Ty, typename = void>
struct IsStringTypeImpl : std::false_type {
};

template <typename _Ty>
struct IsStringTypeImpl<_Ty,
                        std::void_t<std::enable_if_t<std::is_same_v<typename RemoveCVRType<_Ty>::value_type, char> ||
                                                     std::is_same_v<typename RemoveCVRType<_Ty>::value_type, wchar_t>>,
                                    decltype(std::declval<_Ty>().length()),
                                    decltype(std::declval<_Ty>().data())>> : std::true_type {
};

template <typename _Ty>
constexpr bool IsStringType = IsStringTypeImpl<_Ty>::value;

//
// container type -> _Ty::begin() && _Ty::end()
//

template <typename _Ty, typename = void>
struct IsContainer : std::false_type {
};
template <typename _Ty>
struct IsContainer<_Ty, std::void_t<decltype(_Ty().begin()), decltype(_Ty().end())>> : std::true_type {
};
template <typename _Ty>
constexpr bool IsContainerType = IsContainer<_Ty>::value;

//
// map type -> _Ty::mapped_type
//

template <typename _Ty, typename = void>
struct IsMapTypeImpl : std::false_type {
};

template <typename _Ty>
struct IsMapTypeImpl<_Ty, std::void_t<typename RemoveCVRType<_Ty>::mapped_type>> : std::true_type {
};

template <typename _Ty>
constexpr bool IsMapType = IsContainerType<_Ty> &&IsMapTypeImpl<_Ty>::value;

//
// User defined reflect type -> _Ty::REFL::MAKE_FLAG <==> _Ty&
//

template <typename _Ty, typename = void>
struct IsUserReflImpl : std::false_type {
};

template <typename _Ty>
struct IsUserReflImpl<_Ty,
                      std::void_t<std::enable_if_t<
                          std::is_same_v<decltype(typename _Ty::REFL::MAKE_FLAG(std::declval<_Ty &>())), _Ty &>>>>
    : std::true_type {
};

template <typename _Ty>
constexpr bool IsUserRefl = IsUserReflImpl<_Ty>::value;

//
// get type id
//

template <typename _Ty>
constexpr TYPE_ID GetTypeId()
{
    if constexpr (std::is_integral_v<_Ty> || std::is_enum_v<_Ty>) {

        if constexpr (sizeof(_Ty) == 1) {
            return TYPE_ID::BYTE;
        }
        else if constexpr (sizeof(_Ty) == 2) {
            return TYPE_ID::WORD;
        }
        else if constexpr (sizeof(_Ty) == 4) {
            return TYPE_ID::DWORD;
        }
        else if constexpr (sizeof(_Ty) == 8) {
            return TYPE_ID::QWORD;
        }
    }
    else if constexpr (std::is_floating_point_v<_Ty>) {

        if constexpr (std::is_same_v<_Ty, float>) {
            return TYPE_ID::FLOAT;
        }
        else if constexpr (std::is_same_v<_Ty, double>) {
            return TYPE_ID::DOUBLE;
        }
    }
    else if constexpr (IsStringType<_Ty>) {

        if constexpr (std::is_same_v<typename RemoveCVRType<_Ty>::value_type, char>) {
            return TYPE_ID::STRING;
        }
        else if constexpr (std::is_same_v<typename RemoveCVRType<_Ty>::value_type, wchar_t>) {
            return TYPE_ID::WSTRING;
        }
    }
    else if constexpr (std::is_array_v<_Ty>) {

        return TYPE_ID::ARRAY;
    }
    else if constexpr (IsMapType<_Ty>) {

        return TYPE_ID::MAP;
    }
    else if constexpr (std::is_class_v<_Ty>) {

        return TYPE_ID::CLASS;
    }

    return TYPE_ID::UNKNOWN;
}

//
// check initializer to construct struct
//

struct UniversalType {
    template <typename _Ty>
    operator _Ty()
    {
        return {};
    }
};

template <typename _Ty, typename _Initializer, typename = void, typename... _Args>
struct IsInitializerTypeImpl : std::false_type {
};
template <typename _Ty, typename _Initializer, typename... _Args>
struct IsInitializerTypeImpl<_Ty, _Initializer, std::void_t<decltype(_Ty{{_Args{}}..., {_Initializer{}}})>, _Args...>
    : std::true_type {
};
template <typename _Ty, typename _Initializer, typename... _Args>
constexpr bool IsInitializerType = DETAIL::IsInitializerTypeImpl<_Ty, _Initializer, void, _Args...>::value;

struct ADL {

    template <typename _Ty, typename... _Args>
    static constexpr std::size_t counts_impl()
    {
        if constexpr (DETAIL::IsInitializerType<_Ty, UniversalType, _Args...>) {
            return counts_impl<_Ty, _Args..., UniversalType>();
        }
        else {
            return sizeof...(_Args);
        }
    }

    template <size_t _Count, typename _Ty, typename _Visitor>
    static constexpr decltype(auto) VisitsStructImpl(const _Ty &Object, _Visitor &&Visitor)
    {
        static_assert(_Count <= MaxClassMembers, "Too many structure members.");

        if constexpr (_Count == 0) {
            return Visitor();
        }
        else if constexpr (_Count == 1) {
            auto &&[p1] = Object;
            return Visitor(p1);
        }
        else if constexpr (_Count == 2) {
            auto &&[p1, p2] = Object;
            return Visitor(p1, p2);
        }
        else if constexpr (_Count == 3) {
            auto &&[p1, p2, p3] = Object;
            return Visitor(p1, p2, p3);
        }
        else if constexpr (_Count == 4) {
            auto &&[p1, p2, p3, p4] = Object;
            return Visitor(p1, p2, p3, p4);
        }
        else if constexpr (_Count == 5) {
            auto &&[p1, p2, p3, p4, p5] = Object;
            return Visitor(p1, p2, p3, p4, p5);
        }
        else if constexpr (_Count == 6) {
            auto &&[p1, p2, p3, p4, p5, p6] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6);
        }
        else if constexpr (_Count == 7) {
            auto &&[p1, p2, p3, p4, p5, p6, p7] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7);
        }
        else if constexpr (_Count == 8) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8);
        }
        else if constexpr (_Count == 9) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        }
        else if constexpr (_Count == 10) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        }
        else if constexpr (_Count == 11) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        }
        else if constexpr (_Count == 12) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        }
        else if constexpr (_Count == 13) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        }
        else if constexpr (_Count == 14) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
        }
        else if constexpr (_Count == 15) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
        }
        else if constexpr (_Count == 16) {
            auto &&[p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16] = Object;
            return Visitor(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
        }
    }

    template <size_t _Count, typename _Object, typename _Visitor>
    static constexpr decltype(auto) VisitsUserReflImpl(_Object &&Object, _Visitor &&Visitor)
    {
        static_assert(_Count <= MaxClassMembers, "exceed max visit members");

        if constexpr (_Count == 0) {
            return Visitor();
        }
        else if constexpr (_Count == 1) {
            return Visitor(Object.REFL::GET_0(Object));
        }
        else if constexpr (_Count == 2) {
            return Visitor(Object.REFL::GET_0(Object), Object.REFL::GET_1(Object));
        }
        else if constexpr (_Count == 3) {
            return Visitor(Object.REFL::GET_0(Object), Object.REFL::GET_1(Object), Object.REFL::GET_2(Object));
        }
        else if constexpr (_Count == 4) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object));
        }
        else if constexpr (_Count == 5) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object));
        }
        else if constexpr (_Count == 6) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object));
        }
        else if constexpr (_Count == 7) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object));
        }
        else if constexpr (_Count == 8) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object));
        }
        else if constexpr (_Count == 9) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object));
        }
        else if constexpr (_Count == 10) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object));
        }
        else if constexpr (_Count == 11) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object),
                           Object.REFL::GET_10(Object));
        }
        else if constexpr (_Count == 12) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object),
                           Object.REFL::GET_10(Object),
                           Object.REFL::GET_11(Object));
        }
        else if constexpr (_Count == 13) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object),
                           Object.REFL::GET_10(Object),
                           Object.REFL::GET_11(Object),
                           Object.REFL::GET_12(Object));
        }
        else if constexpr (_Count == 14) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object),
                           Object.REFL::GET_10(Object),
                           Object.REFL::GET_11(Object),
                           Object.REFL::GET_12(Object),
                           Object.REFL::GET_13(Object));
        }
        else if constexpr (_Count == 15) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object),
                           Object.REFL::GET_10(Object),
                           Object.REFL::GET_11(Object),
                           Object.REFL::GET_12(Object),
                           Object.REFL::GET_13(Object),
                           Object.REFL::GET_14(Object));
        }
        else if constexpr (_Count == 16) {
            return Visitor(Object.REFL::GET_0(Object),
                           Object.REFL::GET_1(Object),
                           Object.REFL::GET_2(Object),
                           Object.REFL::GET_3(Object),
                           Object.REFL::GET_4(Object),
                           Object.REFL::GET_5(Object),
                           Object.REFL::GET_6(Object),
                           Object.REFL::GET_7(Object),
                           Object.REFL::GET_8(Object),
                           Object.REFL::GET_9(Object),
                           Object.REFL::GET_10(Object),
                           Object.REFL::GET_11(Object),
                           Object.REFL::GET_12(Object),
                           Object.REFL::GET_13(Object),
                           Object.REFL::GET_14(Object),
                           Object.REFL::GET_15(Object));
        }
    }
};

} // namespace DETAIL

using DETAIL::GetTypeId;
using DETAIL::TYPE_ID;

//
// get struct member counts.
//

template <typename _Ty>
constexpr size_t Counts()
{
    using Type = DETAIL::RemoveCVRType<_Ty>;

    if constexpr (DETAIL::IsUserRefl<Type>) {
        return typename _Ty::REFL::COUNTS();
    }
    else {
        return DETAIL::ADL::counts_impl<Type>();
    }
}

//
// visit members
//

template <typename _Ty, typename _Visitor>
constexpr decltype(auto) Visits(_Ty &&Object, _Visitor &&Visitor)
{
    using Type = DETAIL::RemoveCVRType<decltype(Object)>;

    if constexpr (DETAIL::IsUserRefl<Type>) {
        return DETAIL::ADL::VisitsUserReflImpl<Counts<Type>(), _Ty, _Visitor>(std::forward<_Ty>(Object),
                                                                              std::forward<_Visitor>(Visitor));
    }
    else {
        return DETAIL::ADL::VisitsStructImpl<Counts<Type>(), _Ty, _Visitor>(std::forward<_Ty>(Object),
                                                                            std::forward<_Visitor>(Visitor));
    }
}

//
// foreach members
//

template <typename _Ty, typename _Func>
void Foreach(_Ty &&Object, _Func &&Func)
{
    return Visits(
        std::forward<_Ty>(Object),
        [&](auto &&...Items) constexpr { (Func(Items), ...); });
}

//
// Object macro refl
//

#define REFL_ARG_COUNT(...)                                                                                            \
    REFL_MARCO_EXPAND(REFL_INTERNAL_ARG_COUNT(0, ##__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define REFL_INTERNAL_ARG_COUNT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N

#define REFL_CONCAT_(l, r) l##r
#define REFL_CONCAT(l, r)  REFL_CONCAT_(l, r)

#define REFL_MARCO_EXPAND(...) __VA_ARGS__

#define REFL_DOARG0(s, f, o)
#define REFL_DOARG1(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG0(s, f, __VA_ARGS__)) s f(0, t)
#define REFL_DOARG2(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG1(s, f, __VA_ARGS__)) s f(1, t)
#define REFL_DOARG3(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG2(s, f, __VA_ARGS__)) s f(2, t)
#define REFL_DOARG4(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG3(s, f, __VA_ARGS__)) s f(3, t)
#define REFL_DOARG5(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG4(s, f, __VA_ARGS__)) s f(4, t)
#define REFL_DOARG6(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG5(s, f, __VA_ARGS__)) s f(5, t)
#define REFL_DOARG7(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG6(s, f, __VA_ARGS__)) s f(6, t)
#define REFL_DOARG8(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG7(s, f, __VA_ARGS__)) s f(7, t)
#define REFL_DOARG9(s, f, t, ...)  REFL_MARCO_EXPAND(REFL_DOARG8(s, f, __VA_ARGS__)) s f(8, t)
#define REFL_DOARG10(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG9(s, f, __VA_ARGS__)) s f(9, t)
#define REFL_DOARG11(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG10(s, f, __VA_ARGS__)) s f(10, t)
#define REFL_DOARG12(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG11(s, f, __VA_ARGS__)) s f(11, t)
#define REFL_DOARG13(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG12(s, f, __VA_ARGS__)) s f(12, t)
#define REFL_DOARG14(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG13(s, f, __VA_ARGS__)) s f(13, t)
#define REFL_DOARG15(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG14(s, f, __VA_ARGS__)) s f(14, t)
#define REFL_DOARG16(s, f, t, ...) REFL_MARCO_EXPAND(REFL_DOARG15(s, f, __VA_ARGS__)) s f(15, t)

#define REFL_MAKE_ARGS0(_Ty)
#define REFL_MAKE_ARGS1(_Ty)  _Ty
#define REFL_MAKE_ARGS2(_Ty)  REFL_MAKE_ARGS1(_Ty), _Ty
#define REFL_MAKE_ARGS3(_Ty)  REFL_MAKE_ARGS2(_Ty), _Ty
#define REFL_MAKE_ARGS4(_Ty)  REFL_MAKE_ARGS3(_Ty), _Ty
#define REFL_MAKE_ARGS5(_Ty)  REFL_MAKE_ARGS4(_Ty), _Ty
#define REFL_MAKE_ARGS6(_Ty)  REFL_MAKE_ARGS5(_Ty), _Ty
#define REFL_MAKE_ARGS7(_Ty)  REFL_MAKE_ARGS6(_Ty), _Ty
#define REFL_MAKE_ARGS8(_Ty)  REFL_MAKE_ARGS7(_Ty), _Ty
#define REFL_MAKE_ARGS9(_Ty)  REFL_MAKE_ARGS8(_Ty), _Ty
#define REFL_MAKE_ARGS10(_Ty) REFL_MAKE_ARGS9(_Ty), _Ty
#define REFL_MAKE_ARGS11(_Ty) REFL_MAKE_ARGS10(_Ty), _Ty
#define REFL_MAKE_ARGS12(_Ty) REFL_MAKE_ARGS11(_Ty), _Ty
#define REFL_MAKE_ARGS13(_Ty) REFL_MAKE_ARGS12(_Ty), _Ty
#define REFL_MAKE_ARGS14(_Ty) REFL_MAKE_ARGS13(_Ty), _Ty
#define REFL_MAKE_ARGS15(_Ty) REFL_MAKE_ARGS14(_Ty), _Ty
#define REFL_MAKE_ARGS16(_Ty) REFL_MAKE_ARGS15(_Ty), _Ty

#define REFL_MAKE_ARGS(_Ty, Count) REFL_CONCAT(REFL_MAKE_ARGS, Count)(_Ty)

#define REFL_EXPAND_EACH_(sepatator, fun, ...)                                                                         \
    REFL_MARCO_EXPAND(REFL_CONCAT(REFL_DOARG, REFL_ARG_COUNT(__VA_ARGS__))(sepatator, fun, __VA_ARGS__))
#define REFL_EXPAND_EACH(sepatator, fun, ...) REFL_EXPAND_EACH_(sepatator, fun, __VA_ARGS__)

#define REFL_RETURN_NAME(_Idx, X)                                                                                      \
    if constexpr ((_Idx) == _I) {                                                                                      \
        return #X;                                                                                                     \
    }

#define REFL_RETURN_VALUE(_Idx, X)                                                                                     \
    if constexpr ((_Idx) == _I) {                                                                                      \
        return Object.X;                                                                                               \
    }

#define REFL_GET_NAME(_Idx, _Ty)                                                                                       \
    static constexpr auto &GET_NAME_##_Idx()                                                                           \
    {                                                                                                                  \
        return GET_NAME<COUNTS() - 1 - (_Idx)>();                                                                      \
    }

#define REFL_GET_VALUE(_Idx, _Ty)                                                                                      \
    static auto &GET_##_Idx(_Ty &Object)                                                                               \
    {                                                                                                                  \
        return GET<COUNTS() - 1 - (_Idx)>(Object);                                                                     \
    }

#define REFL_GET_VALUE_CONST(_Idx, _Ty)                                                                                \
    static const auto &GET_##_Idx(const _Ty &Object)                                                                   \
    {                                                                                                                  \
        return GET<COUNTS() - 1 - (_Idx)>(Object);                                                                     \
    }

#define MAKE_REFL(_Ty, ...)                                                                                            \
public:                                                                                                                \
    struct REFL {                                                                                                      \
        static _Ty &MAKE_FLAG(_Ty &t)                                                                                  \
        {                                                                                                              \
            return t;                                                                                                  \
        }                                                                                                              \
        static constexpr std::size_t COUNTS()                                                                          \
        {                                                                                                              \
            return REFL_ARG_COUNT(__VA_ARGS__);                                                                        \
        }                                                                                                              \
        template <std::size_t _I>                                                                                      \
        static constexpr auto &GET_NAME()                                                                              \
        {                                                                                                              \
            REFL_EXPAND_EACH(, REFL_RETURN_NAME, __VA_ARGS__)                                                          \
            else                                                                                                       \
            {                                                                                                          \
                static_assert(_I < COUNTS());                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        template <std::size_t _I>                                                                                      \
        static auto &GET(_Ty &Object)                                                                                  \
        {                                                                                                              \
            REFL_EXPAND_EACH(, REFL_RETURN_VALUE, __VA_ARGS__)                                                         \
            else                                                                                                       \
            {                                                                                                          \
                static_assert(_I < COUNTS());                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        template <std::size_t _I>                                                                                      \
        static const auto &GET(const _Ty &Object)                                                                      \
        {                                                                                                              \
            REFL_EXPAND_EACH(, REFL_RETURN_VALUE, __VA_ARGS__)                                                         \
            else                                                                                                       \
            {                                                                                                          \
                static_assert(_I < COUNTS());                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        REFL_EXPAND_EACH(, REFL_GET_NAME, REFL_MAKE_ARGS(_Ty, REFL_ARG_COUNT(__VA_ARGS__)))                            \
        REFL_EXPAND_EACH(, REFL_GET_VALUE, REFL_MAKE_ARGS(_Ty, REFL_ARG_COUNT(__VA_ARGS__)))                           \
        REFL_EXPAND_EACH(, REFL_GET_VALUE_CONST, REFL_MAKE_ARGS(_Ty, REFL_ARG_COUNT(__VA_ARGS__)))                     \
    }

} // namespace REFL
