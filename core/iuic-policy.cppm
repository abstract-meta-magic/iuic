// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.core:policy;
import std;

export namespace iuic::policy {

enum class usage {
  unspecified,
  unused,
  optional,
  required,
};

struct shared {
  unsigned up{0};
};

struct unique {};

struct indexed {};

enum class hovered : std::uint8_t {
  none,
  propagate, // true -> go
  block,     // true -> stop
};

enum class event : std::uint8_t { propagate, block };

}; // namespace iuic::policy
