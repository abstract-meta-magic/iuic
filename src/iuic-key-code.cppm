// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:key_code;
import std;

export namespace iuic {

using key_t = std::uint8_t;

template <typename T, typename U>
concept type_of = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

// Унифицированный код нажатых кнопок
struct key_code {
  template <std::uint8_t size> using key_t_array = key_t[size];
  friend constexpr bool operator==(const key_code &, const key_code &);

  static constexpr std::uint8_t max_key{6};
  static constexpr key_t absence{0};
  static constexpr std::uint64_t group_shift{48};

  constexpr key_code(const key_code &) = default;
  key_code &operator=(const key_code &) = default;
  constexpr key_code(key_code &&) = default;
  key_code &operator=(key_code &&) = default;

  template <std::size_t size>
    requires(size > 0 && size <= max_key)
  constexpr key_code(key_t_array<size> keys) noexcept {
    for (std::size_t i{0}; i < size; ++i) {
      scancode |= static_cast<std::uint64_t>(keys[i]) << (i * 8);
    };
  };

  constexpr key_code(type_of<std::vector<key_t>> auto &&keys) noexcept {

    if (keys.size() > max_key)
      return;

    for (std::size_t i{0}; i < keys.size(); ++i) {
      scancode |= static_cast<std::uint64_t>(keys[i]) << (i * 8);
    }
  };

  template <std::size_t size>
    requires(size > 0 && size <= max_key)
  constexpr key_code(std::array<key_t, size> keys) noexcept {
    for (std::size_t i{0}; i < size; ++i) {
      scancode |= static_cast<std::uint64_t>(keys[i]) << (i * 8);
    };
  };

  constexpr key_code() noexcept {};

  constexpr key_code(type_of<key_t> auto &&...keys) noexcept
      : key_code{std::array<key_t, sizeof...(keys)>{
            std::forward<decltype(keys)>(keys)...}} {}
  void set_group(key_t group) {
    scancode |= static_cast<std::uint64_t>(group) << group_shift;
  };

  std::uint8_t get_group() const noexcept {
    return static_cast<std::uint8_t>((scancode >> group_shift) & 0xFF);
  }

  // issure : не возвращает нужеый code
  key_t operator[](std::uint8_t index) {
    if (index >= max_key) {
      return 0x00;
    }

    return static_cast<std::uint8_t>((scancode >> (index * 8) & 0xFF));
  };

private:
  // [1-6] key_t; [7] group_id; [8] - undefined
  std::uint64_t scancode{0};
};

constexpr bool operator==(const key_code &lhs, const key_code &rhs) {
  return lhs.scancode == rhs.scancode;
};

constexpr bool operator!=(const key_code &lhs, const key_code &rhs) {
  return not(lhs == rhs);
};

namespace keymap::en {

key_code qwerty(std::same_as<const char *> auto... args) { return {}; };
} // namespace keymap::en

}; // namespace iuic
