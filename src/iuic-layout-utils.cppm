// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

// TODO : может переименовать в layout.utils ?
export module iuic.core:layout.utils;
import std;
import iuic.underlying;
import :layout.def;
import :environment.tmp;
import :scheme.base;

namespace iuic {

export namespace layout {
struct utils_base {
  utils_base(environment::tmp &tenv_, scheme::blueprint::iterator it_)
      : tenv{tenv_}, it{it_} {}

  style::sid self() { return utils::tree::access_iterator{it}->sid; };

  style::sid parent() {
    utils::tree::root_iterator rit{it};
    if (++rit) {
      return rit->sid;
    }
    return style::sid{0};
  };

  style::value style_of(style::sid sid) { return tenv.style.get(sid); };

  units::ui::size root_size();

protected:
  environment::tmp &tenv;
  scheme::blueprint::iterator it;
};

namespace measure {

/*
Measure считаеться для
bordered rect
*/
using result = units::ui::adaptive::size;

struct unit {
  style::sid sid{0};
  result measure{units::ui::none{}, units::ui::none{}};
};

using tree = utils::tree::flat_bfs_type<unit>;

struct frame_utils : public utils_base {
  struct childs_proxy {
    const measure::tree::const_sibling_iterator b;
    const utils::tree::sentinel<measure::tree> e;
    measure::tree::const_sibling_iterator begin() { return b; };
    utils::tree::sentinel<measure::tree> end() { return e; };
  };

  childs_proxy childs_range() { return {ch, {}}; };

private:
  measure::tree::sibling_iterator ch;
};
struct text_utils : public utils_base {};
}; // namespace measure

namespace arrange {
struct frame_utils : public utils_base {
  struct childs_proxy {
    measure::tree::const_sibling_iterator b;
    utils::tree::sentinel<measure::tree> e;
    measure::tree::const_sibling_iterator begin() { return b; };
    utils::tree::sentinel<measure::tree> end() { return e; };
  };

  childs_proxy childs_range() { return {ch, {}}; };

  void apply_element(measure::tree::const_sibling_iterator it_,
                     units::ui::area a) {
    utils::tree::access_iterator ait{utils::tree::shift(it, it_)};

    if (ait) {
      ait->area = a;
      ait->meta.set(ait->meta.applyed);
    }
  };

private:
  measure::tree::sibling_iterator ch;
};

struct text_utils : public utils_base {};
}; // namespace arrange

struct frame {
  virtual ~frame() {};
  virtual measure::result measure(measure::frame_utils utils) = 0;
  virtual bool arrange(arrange::frame_utils utils) = 0;
};

struct text {
  virtual ~text() {};
  virtual measure::result measure(measure::text_utils utils) = 0;
  virtual iuic::text::glyph::sequence arrange(arrange::text_utils utils) = 0;
};
} // namespace layout
} // namespace iuic
// background\foreground
