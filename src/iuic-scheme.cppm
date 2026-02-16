// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.core:scheme;
import std;
import iuic.underlying;
import :policy;
import :style;
import :text.present;
import :event;
// env
//
namespace iuic {
namespace scheme {
// MB transform to SAO presentation
struct element {
  units::ui::area area; // x,y w,h
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
};

struct base_context {
  const style::ref &style_of(units::uid);

  policy::hovered hovered_policy_of(units::uid);

  policy::event event_policy_of(units::uid);

  const units::ui::area &area_of(units::uid);

  bool has_state(units::uid, state::value);

  units::uid self();

  units::uid parent_of(units::uid);

  std::vector<units::uid> childs_of(units::uid);

  std::span<const units::uid> get_selected();

  void reset_selected();

  void select(units::uid);

  units::ui::position get_pointer_position();

  key_code get_key_code();

private:
  environment::persist &env;
  std::vector<units::uid> selected__;
  // element_tree::base_iterator it;
};

struct el {
  units::ui::rect borderless_rect; // x,y w,h
  units::ui::rect bordered_rect;   // x,y w,h
  style::cref style;
  units::uid uid;
};

export struct frame {
  units::ui::rect rect;
  style::cref style;
};

export struct text {
  units::ui::rect rect;
  iuic::text::glyph::sequence text;
  style::cref style;
};

namespace eval {

export struct context : base_context {

  void attach_state(units::uid uid, state::value);
};

template <typename T>
concept visitor = requires(T obj, context &ctx) { obj(ctx); };
} // namespace eval

namespace reval {
//
//
export struct context {};
template <typename T>
concept visitor = requires(T obj, context &ctx) { obj(ctx); };
} // namespace reval

// global scheme eval
namespace geval {
export struct context {

  void set_pointer_position(units::ui::position);

  void set_key_code(key_code);
};
template <typename T>
concept visitor = requires(T obj, context &ctx) { obj(ctx); };
}; // namespace geval

namespace proc {
//
//

export struct context : base_context {
  void machine_of(units::uid);

  std::span<event::value> events_of(units::uid);
};

template <typename T>
concept visitor = requires(T obj, context &ctx) { obj(ctx); };
}; // namespace proc

namespace rproc {
//
//

export struct context : base_context {
  void machine_of(units::uid);

  std::span<const event::value> events_of(units::uid);

  void trigger(const event::value &e);
};
template <typename T>
concept visitor = requires(T obj, context &ctx) { obj(ctx); };
}; // namespace rproc

namespace ordered {
//
//

export struct context {
  void get_unit();

  void get_element();
};
template <typename T>
concept visitor = requires(T obj, context &ctx) { obj(ctx); };
}; // namespace ordered

namespace explore {
template <typename T>
concept visitor = geval::visitor<T> || eval::visitor<T> || reval::visitor<T> ||
                  proc::visitor<T> || rproc::visitor<T> || ordered::visitor<T>;
}
/*
Сначала будет неоптимальный
пошаговый обход.
На дистанций упорядоченный параллельный\асинхронный обход.
 */
export struct explorer {

  void explore(explore::visitor auto &&...visitors) {};

private:
  std::vector<element> elements;
  std::vector<event::value> events;
  units::hash hash;
};

} // namespace scheme

}; // namespace iuic
