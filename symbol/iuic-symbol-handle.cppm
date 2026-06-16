// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.symbol:handle;
import std;

namespace iuic::symbol {

export enum class scheme_handle : std::uint64_t {};
export enum class pack_handle : std::uint64_t {};

enum class slot_handle : std::uint64_t {};
enum class link_handle : std::uint64_t {};
enum class lock_handle : std::uint64_t {};

template <typename T>
constexpr T invalid_handle =
    static_cast<T>(std::numeric_limits<std::underlying_type_t<T>>::max());

}; // namespace iuic::symbol
