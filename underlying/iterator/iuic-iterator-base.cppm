// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:base;
import std;

export namespace iuic::iterator {

template <typename Container, typename... Ex> struct common;

template <typename Container, typename Mode, typename... Ex> struct underlying;

}; // namespace iuic::iterator

// [MODS]
export namespace iuic::iterator::mode {

struct tag_t {};

struct basic_t : tag_t {

} basic;

struct liner_t : tag_t {
} liner{};

struct reverse_t : tag_t {
} reverse{};

struct index_t : tag_t {
} index{};

struct iterator_t : tag_t {
} iterator{};

struct root_t : tag_t {
} root{};

struct sibling_t : tag_t {
} sibling{};

}; // namespace iuic::iterator::mode

// [EX]
export namespace iuic::iterator::extension {

struct tag_t {};

template <template <typename...> typename Iterator>
struct iterator_t : tag_t {};

template <template <typename... Ts> typename Iterator>
constexpr iterator_t<Iterator> iterator;

struct debug_t : tag_t {
} debug;

}; // namespace iuic::iterator::extension

namespace iuic::iterator {

template <typename T>
concept is_iterator_mode =
    std::is_base_of_v<std::remove_all_extents_t<T>, mode::tag_t>;

template <typename T>
concept is_iterator_extension =
    std::is_base_of_v<std::remove_all_extents_t<T>, extension::tag_t>;

}; // namespace iuic::iterator
