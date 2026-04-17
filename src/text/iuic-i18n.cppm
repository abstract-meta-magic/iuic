// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:i18n;
import std;
import iuic.underlying;

namespace iuic::text::i18n {
struct uniform_string {};
}; // namespace iuic::text::i18n

export namespace iuic::text::i18n {

struct key {
  key(std::string_view);

  std::expected<std::string_view, int> as_utf8() const;
  std::expected<std::string_view, int> as_utf32() const;
  std::expected<std::string_view, int> as_utf64() const;

  void assign();

  bool exist();

  int type();
};

}; // namespace iuic::text::i18n
