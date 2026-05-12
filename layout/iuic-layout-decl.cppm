// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:decl;
import std;
import iuic.underlying;
import iuic.text;
import iuic.style;

export namespace iuic::layout {

struct element {
  units::ui::adaptive::size measure;
  units::ui::area arrange;
  std::span<const text::present::token> text;
  style::value style;

  struct meta {
    void set_discarted() { value.set(static_cast<int>(map::discarted), true); };

    void set_virtualized() {
      value.set(static_cast<int>(map::virtualized), true);
    };

    void set_text_mark() { value.set(static_cast<int>(map::text_mark), true); };

    void set_applyed() { value.set(static_cast<int>(map::applyed), true); };

    void set_measured() { value.set(static_cast<int>(map::measured), true); };

    void set_arranged() { value.set(static_cast<int>(map::arranged), true); };

    bool is_discarted() {
      return value.test(static_cast<int>(map::discarted));
    };

    bool is_virtualized() {
      return value.test(static_cast<int>(map::virtualized));
    };

    bool is_applyed() { return value.test(static_cast<int>(map::applyed)); };

    bool is_measured() { return value.test(static_cast<int>(map::measured)); };

    bool is_arranged() { return value.test(static_cast<int>(map::arranged)); };

    bool is_text() { return value.test(static_cast<int>(map::text_mark)); };

  private:
    enum class map {
      discarted = 0,
      text_mark,
      virtualized,
      measured,
      applyed,
      arranged,
      size__,
    };
    std::bitset<static_cast<std::size_t>(map::size__)> value{0};
  } meta;
};

using tree_type = ::iuic::tree::flat_bfs_type<element>;

}; // namespace iuic::layout
