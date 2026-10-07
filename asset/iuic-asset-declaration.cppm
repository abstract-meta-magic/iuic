// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:declaration;
import std;
import :base;

namespace iuic::asset {

export struct declaration {
  constexpr declaration() : data{std::string_view{""}}, hash{0} {};

  explicit constexpr declaration(std::string_view v) : data{v}, hash{0} {
    set_hash__();
  };

  template <std::size_t S>
  consteval declaration(const char (&ch)[S])
      : declaration{std::string_view{ch, S - 1}} {}

  constexpr explicit declaration(const std::string &s) : data{s}, hash{0} {
    set_hash__();
  };

  constexpr explicit declaration(std::string &&s)
      : data{std::move(s)}, hash{0} {
    set_hash__();
  };

  constexpr std::string_view get_name() const noexcept {
    return std::visit([](auto &&data) -> std::string_view { return data; },
                      data);
  };

  constexpr hash_t get_hash() const noexcept { return hash; };

private:
  constexpr void set_hash__() noexcept { hash = hash::from(get_name()); };

private:
  std::variant<std::string, std::string_view> data;
  hash_t hash;
};

constexpr bool operator==(const declaration &lhs,
                          const declaration &rhs) noexcept {
  return lhs.get_name() == rhs.get_name();
};

static_assert(declaration{"font.base"}.get_hash() ==
                  declaration{"font.base"}.get_hash(),
              "Wrong hash");

}; // namespace iuic::asset
