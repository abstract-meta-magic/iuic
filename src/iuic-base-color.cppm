
module;

#include <cstdint>

export module iuic.core:base.color;
export import :base;

// use https://www.w3schools.com/cssref/css_colors.php
export namespace iuic::color::css {

struct white : color_t {
  constexpr white() : color_t{255, 255, 255, 255} {}
  explicit constexpr white(std::uint8_t alpha)
      : color_t{255, 255, 255, alpha} {}
};

struct black : color_t {
  constexpr black() : color_t{0, 0, 0, 255} {}
  explicit constexpr black(std::uint8_t alpha) : color_t{0, 0, 0, alpha} {}
};

struct red : color_t {
  constexpr red() : color_t{255, 0, 0, 255} {}
  explicit constexpr red(std::uint8_t alpha) : color_t{255, 0, 0, alpha} {}
};

struct green : color_t {
  constexpr green() : color_t{0, 128, 0, 255} {}
  explicit constexpr green(std::uint8_t alpha) : color_t{0, 128, 0, alpha} {}
};

struct blue : color_t {
  constexpr blue() : color_t{0, 0, 255, 255} {}
  explicit constexpr blue(std::uint8_t alpha) : color_t{0, 0, 255, alpha} {}
};

struct gray : color_t {
  constexpr gray() : color_t{128, 128, 128, 255} {}
  explicit constexpr gray(std::uint8_t alpha) : color_t{128, 128, 128, alpha} {}
};

struct lightgray : color_t {
  constexpr lightgray() : color_t{211, 211, 211, 255} {}
  explicit constexpr lightgray(std::uint8_t alpha)
      : color_t{211, 211, 211, alpha} {}
};

struct darkgray : color_t {
  constexpr darkgray() : color_t{169, 169, 169, 255} {}
  explicit constexpr darkgray(std::uint8_t alpha)
      : color_t{169, 169, 169, alpha} {}
};

struct yellow : color_t {
  constexpr yellow() : color_t{255, 255, 0, 255} {}
  explicit constexpr yellow(std::uint8_t alpha) : color_t{255, 255, 0, alpha} {}
};

struct cyan : color_t {
  constexpr cyan() : color_t{0, 255, 255, 255} {}
  explicit constexpr cyan(std::uint8_t alpha) : color_t{0, 255, 255, alpha} {}
};

struct magenta : color_t {
  constexpr magenta() : color_t{255, 0, 255, 255} {}
  explicit constexpr magenta(std::uint8_t alpha)
      : color_t{255, 0, 255, alpha} {}
};

struct orange : color_t {
  constexpr orange() : color_t{255, 165, 0, 255} {}
  explicit constexpr orange(std::uint8_t alpha) : color_t{255, 165, 0, alpha} {}
};

struct purple : color_t {
  constexpr purple() : color_t{128, 0, 128, 255} {}
  explicit constexpr purple(std::uint8_t alpha) : color_t{128, 0, 128, alpha} {}
};

struct brown : color_t {
  constexpr brown() : color_t{165, 42, 42, 255} {}
  explicit constexpr brown(std::uint8_t alpha) : color_t{165, 42, 42, alpha} {}
};

struct pink : color_t {
  constexpr pink() : color_t{255, 192, 203, 255} {}
  explicit constexpr pink(std::uint8_t alpha) : color_t{255, 192, 203, alpha} {}
};

struct lime : color_t {
  constexpr lime() : color_t{0, 255, 0, 255} {}
  explicit constexpr lime(std::uint8_t alpha) : color_t{0, 255, 0, alpha} {}
};

struct teal : color_t {
  constexpr teal() : color_t{0, 128, 128, 255} {}
  explicit constexpr teal(std::uint8_t alpha) : color_t{0, 128, 128, alpha} {}
};
}; // namespace iuic::color::css
