// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:static_instance;
import std;
import :base;
import :declaration;

namespace iuic::asset {

// decl@instance
struct static_instance {

  consteval static_instance()
      : decl{}, name_{std::string_view{""}}, hash_{0} {};

  template <std::size_t S>
  consteval static_instance(const char (&name)[S])
      : decl{extract_decl_string_ct(name)},
        name_{extract_instance_string_ct(name)}, hash_{0} {
    hash_ = hash::from(std::string_view{name, S - 1}); // full str hash
  };

  constexpr const declaration &of() const { return decl; };

  constexpr std::string_view get_name() const {
    return std::visit([](auto &obj) -> std::string_view { return obj; }, name_);
  };

  constexpr hash_t get_hash() const { return hash_; };

private:
  // TODO : pritty
  constexpr std::string_view extract_decl_string_ct(std::span<const char> str) {
    if (auto res = std::find(str.begin(), str.end(), '@'); res != str.end()) {
      auto dst = std::distance(str.begin(), res);
      return std::string_view(&str[0], dst);
    }
    throw std::logic_error{"not foud separaor '@'"};
  };

  // TODO : pritty
  constexpr std::string_view
  extract_instance_string_ct(std::span<const char> str) {
    if (auto res = std::find(str.begin(), str.end(), '@'); res != str.end()) {
      auto dst = std::distance(str.begin(), res);
      // '\0' + '@' == 2
      static constexpr std::size_t char_skip{2};
      return std::string_view(&str[dst + 1], str.size() - dst - char_skip);
    }
    throw std::logic_error{"not foud separaor '@'"};
  };

private:
  declaration decl;
  std::variant<std::string, std::string_view> name_;
  hash_t hash_;
};

constexpr bool operator==(const static_instance &lhs,
                          const static_instance &rhs) {
  return lhs.get_hash() == rhs.get_hash();
};

}; // namespace iuic::asset
