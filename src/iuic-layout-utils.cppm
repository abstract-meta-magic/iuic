// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:layout.utils;
import std;
import iuic.underlying;
import iuic.text;
import :layout.def;
import :environment.tmp;
import :scheme.base;

namespace iuic {

export namespace layout {
struct utils_base {
  using iterator = scheme::iterators::base;
  using root = scheme::iterators::root;
  using sibling = scheme::iterators::sibling;

  utils_base(environment::tmp &tenv_, scheme::blueprint::base_iterator it_)
      : tenv{tenv_}, it{it_} {}

  iterator self() { return it; };

  iterator parent() { return ++utils::tree::root_iterator{it}; };

  utils::tree::iterator_range_for<sibling> childs() {
    return {scheme::iterators::childs_of(it)};
  };

  // this is joke...BUT
  struct : utils::adv_member_for<utils_base> {
    style::value operator[](iterator el) {
      return self().tenv.style.get(
          utils::tree::access_iterator{utils::tree::shift(self().it, el)}->sid);
    };
  } style [[no_unique_address]];

  units::ui::size root_size() { return tenv.meta.viewport_size; };

protected:
  environment::tmp &tenv;
  scheme::blueprint::base_iterator it;
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
  frame_utils(environment::tmp &tenv, scheme::blueprint::base_iterator it,
              measure::tree::sibling_iterator m_)
      : utils_base{tenv, it}, m{m_} {};

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const measure::result &operator[](iterator el) {
      return utils::tree::access_iterator{utils::tree::shift(self().m, el)}
          ->measure;
    };
  } measure [[no_unique_address]];

private:
  measure::tree::sibling_iterator m;
};

struct text_utils : public utils_base {};
}; // namespace measure

namespace arrange {
struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, scheme::blueprint::base_iterator it,
              measure::tree::sibling_iterator m_)
      : utils_base{tenv, it}, m{m_} {};

  void apply_element(iterator el, units::ui::area a) {
    utils::tree::access_iterator ait{utils::tree::shift(it, el)};

    if (ait) {
      ait->area = a;
      ait->meta.set(ait->meta.applied);
    }
  };

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const measure::result &operator[](iterator el) {
      return utils::tree::access_iterator{utils::tree::shift(self().m, el)}
          ->measure;
    };
  } measure [[no_unique_address]];

  const units::ui::area &self_area() const {
    return (utils::tree::access_iterator{it})->area;
  };

private:
  measure::tree::sibling_iterator m;
};

struct text_utils : public utils_base {};
}; // namespace arrange

struct frame {
  constexpr virtual ~frame() {};

  constexpr frame() noexcept {};

  virtual std::optional<measure::result>
  measure(measure::frame_utils utils) const = 0;

  virtual bool arrange(arrange::frame_utils utils) const = 0;
};

struct text {
  virtual ~text() {};
  virtual std::optional<measure::result>
  measure(measure::text_utils utils) const = 0;

  virtual std::span<const iuic::text::present::token>
  arrange(arrange::text_utils utils) const = 0;
};
} // namespace layout
} // namespace iuic
// background\foreground
