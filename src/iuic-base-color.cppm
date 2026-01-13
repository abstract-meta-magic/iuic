// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:base.color;
import std;
import iuic.underlying;

// use https://www.w3schools.com/cssref/css_colors.php
export namespace iuic::color::css {

struct white : units::color_t {
  constexpr white() : units::color_t{255, 255, 255, 255} {}
  explicit constexpr white(std::uint8_t alpha)
      : units::color_t{255, 255, 255, alpha} {}
};

struct black : units::color_t {
  constexpr black() : units::color_t{0, 0, 0, 255} {}
  explicit constexpr black(std::uint8_t alpha)
      : units::color_t{0, 0, 0, alpha} {}
};

struct red : units::color_t {
  constexpr red() : units::color_t{255, 0, 0, 255} {}
  explicit constexpr red(std::uint8_t alpha)
      : units::color_t{255, 0, 0, alpha} {}
};

struct green : units::color_t {
  constexpr green() : units::color_t{0, 128, 0, 255} {}
  explicit constexpr green(std::uint8_t alpha)
      : units::color_t{0, 128, 0, alpha} {}
};

struct blue : units::color_t {
  constexpr blue() : units::color_t{0, 0, 255, 255} {}
  explicit constexpr blue(std::uint8_t alpha)
      : units::color_t{0, 0, 255, alpha} {}
};

struct gray : units::color_t {
  constexpr gray() : units::color_t{128, 128, 128, 255} {}
  explicit constexpr gray(std::uint8_t alpha)
      : units::color_t{128, 128, 128, alpha} {}
};

struct lightgray : units::color_t {
  constexpr lightgray() : units::color_t{211, 211, 211, 255} {}
  explicit constexpr lightgray(std::uint8_t alpha)
      : units::color_t{211, 211, 211, alpha} {}
};

struct darkgray : units::color_t {
  constexpr darkgray() : units::color_t{169, 169, 169, 255} {}
  explicit constexpr darkgray(std::uint8_t alpha)
      : units::color_t{169, 169, 169, alpha} {}
};

struct yellow : units::color_t {
  constexpr yellow() : units::color_t{255, 255, 0, 255} {}
  explicit constexpr yellow(std::uint8_t alpha)
      : units::color_t{255, 255, 0, alpha} {}
};

struct cyan : units::color_t {
  constexpr cyan() : units::color_t{0, 255, 255, 255} {}
  explicit constexpr cyan(std::uint8_t alpha)
      : units::color_t{0, 255, 255, alpha} {}
};

struct magenta : units::color_t {
  constexpr magenta() : units::color_t{255, 0, 255, 255} {}
  explicit constexpr magenta(std::uint8_t alpha)
      : units::color_t{255, 0, 255, alpha} {}
};

struct orange : units::color_t {
  constexpr orange() : units::color_t{255, 165, 0, 255} {}
  explicit constexpr orange(std::uint8_t alpha)
      : units::color_t{255, 165, 0, alpha} {}
};

struct purple : units::color_t {
  constexpr purple() : units::color_t{128, 0, 128, 255} {}
  explicit constexpr purple(std::uint8_t alpha)
      : units::color_t{128, 0, 128, alpha} {}
};

struct brown : units::color_t {
  constexpr brown() : units::color_t{165, 42, 42, 255} {}
  explicit constexpr brown(std::uint8_t alpha)
      : units::color_t{165, 42, 42, alpha} {}
};

struct pink : units::color_t {
  constexpr pink() : units::color_t{255, 192, 203, 255} {}
  explicit constexpr pink(std::uint8_t alpha)
      : units::color_t{255, 192, 203, alpha} {}
};

struct lime : units::color_t {
  constexpr lime() : units::color_t{0, 255, 0, 255} {}
  explicit constexpr lime(std::uint8_t alpha)
      : units::color_t{0, 255, 0, alpha} {}
};

struct teal : units::color_t {
  constexpr teal() : units::color_t{0, 128, 128, 255} {}
  explicit constexpr teal(std::uint8_t alpha)
      : units::color_t{0, 128, 128, alpha} {}
};
}; // namespace iuic::color::css
