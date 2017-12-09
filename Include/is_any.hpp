#pragma once

template<typename T, typename... Rest>
struct is_any : std::false_type {};

template<typename T, typename First>
struct is_any<T, First> : std::is_same<T, First> {};

template<typename T, typename First, typename... Rest>
struct is_any<T, First, Rest...>
    : std::integral_constant<bool, std::is_same<T, First>::value || is_any<T, Rest...>::value>
{};
template<template<typename,typename>class checker, typename... Ts>
struct is_any_to_first : std::false_type {};

template<template<typename,typename>class checker, typename T0, typename T1, typename... Ts>
struct is_any_to_first<checker, T0, T1, Ts...> :
  std::integral_constant< bool, checker<T0, T1>::value || is_any_to_first<checker, T0, Ts...>::value>
{};
template<typename... Ts>
using is_any_same_to_first = is_any_to_first< std::is_same, Ts... >;

template<template<typename,typename>class checker, typename... Ts>
struct is_all : std::true_type {};

template<template<typename,typename>class checker, typename T0, typename T1, typename... Ts>
struct is_all<checker, T0, T1, Ts...> :
  std::integral_constant< bool, checker<T0, T1>::value && is_all<checker, T0, Ts...>::value>
{};

template<typename... Ts>
using is_all_same = is_all< std::is_same, Ts... >;

