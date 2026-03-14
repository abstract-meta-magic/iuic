// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:base.color;
import std;
import iuic.underlying;

// use https://www.w3schools.com/cssref/css_colors.php
export namespace iuic::color::css {

struct white : units::color {
  constexpr white() : units::color{255, 255, 255, 255} {}
  explicit constexpr white(std::uint8_t alpha)
      : units::color{255, 255, 255, alpha} {}
};

struct black : units::color {
  constexpr black() : units::color{0, 0, 0, 255} {}
  explicit constexpr black(std::uint8_t alpha) : units::color{0, 0, 0, alpha} {}
};

struct red : units::color {
  constexpr red() : units::color{255, 0, 0, 255} {}
  explicit constexpr red(std::uint8_t alpha) : units::color{255, 0, 0, alpha} {}
};

struct green : units::color {
  constexpr green() : units::color{0, 128, 0, 255} {}
  explicit constexpr green(std::uint8_t alpha)
      : units::color{0, 128, 0, alpha} {}
};

struct blue : units::color {
  constexpr blue() : units::color{0, 0, 255, 255} {}
  explicit constexpr blue(std::uint8_t alpha)
      : units::color{0, 0, 255, alpha} {}
};

struct gray : units::color {
  constexpr gray() : units::color{128, 128, 128, 255} {}
  explicit constexpr gray(std::uint8_t alpha)
      : units::color{128, 128, 128, alpha} {}
};

struct lightgray : units::color {
  constexpr lightgray() : units::color{211, 211, 211, 255} {}
  explicit constexpr lightgray(std::uint8_t alpha)
      : units::color{211, 211, 211, alpha} {}
};

struct darkgray : units::color {
  constexpr darkgray() : units::color{169, 169, 169, 255} {}
  explicit constexpr darkgray(std::uint8_t alpha)
      : units::color{169, 169, 169, alpha} {}
};

struct yellow : units::color {
  constexpr yellow() : units::color{255, 255, 0, 255} {}
  explicit constexpr yellow(std::uint8_t alpha)
      : units::color{255, 255, 0, alpha} {}
};

struct cyan : units::color {
  constexpr cyan() : units::color{0, 255, 255, 255} {}
  explicit constexpr cyan(std::uint8_t alpha)
      : units::color{0, 255, 255, alpha} {}
};

struct magenta : units::color {
  constexpr magenta() : units::color{255, 0, 255, 255} {}
  explicit constexpr magenta(std::uint8_t alpha)
      : units::color{255, 0, 255, alpha} {}
};

struct orange : units::color {
  constexpr orange() : units::color{255, 165, 0, 255} {}
  explicit constexpr orange(std::uint8_t alpha)
      : units::color{255, 165, 0, alpha} {}
};

struct purple : units::color {
  constexpr purple() : units::color{128, 0, 128, 255} {}
  explicit constexpr purple(std::uint8_t alpha)
      : units::color{128, 0, 128, alpha} {}
};

struct brown : units::color {
  constexpr brown() : units::color{165, 42, 42, 255} {}
  explicit constexpr brown(std::uint8_t alpha)
      : units::color{165, 42, 42, alpha} {}
};

struct pink : units::color {
  constexpr pink() : units::color{255, 192, 203, 255} {}
  explicit constexpr pink(std::uint8_t alpha)
      : units::color{255, 192, 203, alpha} {}
};

struct lime : units::color {
  constexpr lime() : units::color{0, 255, 0, 255} {}
  explicit constexpr lime(std::uint8_t alpha)
      : units::color{0, 255, 0, alpha} {}
};

struct teal : units::color {
  constexpr teal() : units::color{0, 128, 128, 255} {}
  explicit constexpr teal(std::uint8_t alpha)
      : units::color{0, 128, 128, alpha} {}
};
}; // namespace iuic::color::css
