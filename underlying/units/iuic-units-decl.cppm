// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.units:decl;
import std;

export namespace iuic::units {

struct color {
  std::uint8_t r{0}, g{0}, b{0}, a{255};
};
enum class percent : std::uint8_t {};

// global setted segment size in upixel
enum class segment : std::uint32_t {};

enum class vh : std::uint8_t {};

enum class vw : std::uint8_t {};

enum class pixel : std::int32_t {};

enum class upixel : std::uint32_t {};

enum class uid : std::uint64_t {};

enum class hash : std::uint64_t {};

enum class weight : std::uint64_t {};

struct angle {
  // Brads + LUT ???
  enum class impl_t : std::uint16_t {};

  constexpr pixel sin();

  constexpr pixel cos();

  impl_t value;
};

namespace literals {

constexpr inline pixel operator""_px(unsigned long long value) {
  return pixel{static_cast<std::int32_t>(value)};
};

constexpr inline uid operator""_uid(unsigned long long value) {
  return uid{static_cast<std::uint64_t>(value)};
};

constexpr inline segment operator""_seg(unsigned long long value) {
  return segment{static_cast<std::uint32_t>(value)};
};

constexpr inline upixel operator""_upx(unsigned long long value) {
  return upixel{static_cast<std::uint32_t>(value)};
};

constexpr inline percent operator""_per(unsigned long long value) {
  if (value > 250) {
    throw std::out_of_range{"range 0-250"};
  };
  return percent{static_cast<std::uint8_t>(value)};
};

constexpr inline vh operator""_vh(unsigned long long value) {
  if (value > 250) {
    throw std::out_of_range{"range 0-250"};
  };
  return vh{static_cast<std::uint8_t>(value)};
};

constexpr inline vw operator""_vw(unsigned long long value) {
  if (value > 250) {
    throw std::out_of_range{"range 0-250"};
  };
  return vw{static_cast<std::uint8_t>(value)};
};

constexpr inline weight operator""_w(unsigned long long value) {
  return weight{static_cast<std::uint64_t>(value)};
};

consteval inline color operator""_rgb8(const char *cstr, std::size_t size) {
  if (auto name = std::string_view{cstr, size}; name == "black") {
    return {0, 0, 0, 255};
  } else if (name == "white") {
    return {255, 255, 255, 255};
  } else if (name == "pink") {
    return {255, 139, 150, 255};
  } else if (name == "orange") {
    return {246, 134, 0, 255};
  } else if (name == "brown") {
    return {130, 76, 48, 255};
  } else if (name == "yellow") {
    return {233, 191, 0, 255};
  } else if (name == "olive") {
    return {94, 85, 7, 255};
  } else if (name == "green") {
    return {0, 144, 108, 255};
  } else if (name == "blue") {
    return {0, 105, 165, 255};
  } else if (name == "purple") {
    return {143, 0, 165, 255};
  } else if (name == "gray") {
    return {135, 134, 134, 255};
  } else {
    throw "Undefined color";
  }
};

}; // namespace literals
}; // namespace iuic::units
