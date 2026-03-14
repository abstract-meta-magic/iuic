// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.explorer;
import std;
import iuic.underlying;
import :policy;
import :style;
import :text.present;
import :event;
import :scheme.base;

namespace iuic {
namespace scheme {

struct base_context {
  // style::value style_of(units::uid);

  // policy::hovered hovered_policy_of(units::uid);

  // policy::event event_policy_of(units::uid);

  // bool has_state(units::uid, state::value);

  units::uid self_uid() const {
    return (utils::tree::access_iterator{it__})->uid;
  };

  const units::ui::area &self_area() const {
    return (utils::tree::access_iterator{it__})->area;
  };

  bool has_state(state::value value) {
    return penv.state.has(utils::tree::access_iterator{it__}->uid, value);
  };

  style::value self_style() {
    return tenv.style.get(utils::tree::access_iterator{it__}->sid);
  };

  // units::uid parent_of(units::uid);

  // std::vector<units::uid> childs_of(units::uid);

  // std::span<const units::uid> get_selected();

  // void reset_selected();

  // void select(units::uid);

  // units::ui::position get_pointer_position();

  // key_code get_key_code();

  base_context(blueprint::base_iterator it, environment::tmp &tenv_,
               environment::persist &penv_)
      : it__{it}, tenv{tenv_}, penv{penv_} {};

protected:
  blueprint::base_iterator it__;
  environment::tmp &tenv;
  environment::persist &penv;
};

export struct frame {
  units::ui::rect rect;
  style::value style;
};

export struct text {
  units::ui::rect rect;
  iuic::text::glyph::sequence text;
  style::value style;
};

namespace eval {

export struct context : base_context {
  context(blueprint::base_iterator it, environment::tmp &tenv_,
          environment::persist &penv_)
      : base_context{it, tenv_, penv_} {};
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
export struct context : base_context {
  context(blueprint::base_iterator it, environment::tmp &tenv,
          environment::persist &penv)
      : base_context{it, tenv, penv} {}

  void set_key(key_code code) { penv.external.key_code = code; };

  void attach_state(units::uid uid, state::value value) {
    penv.state.attach(uid, value);
  };

  void detach_state(units::uid uid, state::value value) {
    penv.state.detach(uid, value);
  };

  std::span<event::value> events_of(units::uid uid) {
    return tenv.event.list_of(uid);
  };

  void event_trigger(event::value value) { event::trigger(value, penv); };
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

export struct explorer {

  void explore(explore::visitor auto &&...visitors) {

    auto itrange = utils::tree::bfs_iterator_range_for{
        data, utils::tree::iterator_type<utils::tree::access_iterator>{}};

    for (auto ait : itrange) {
      if (not ait->meta.has(ait->meta.discarded)) {
        (visit(ait, visitors), ...);
      }
    }

    (
        [&]<typename type>(const type &obj) {
          if constexpr (geval::visitor<type>) {
            geval::context ctx{{}, tenv, penv};
            obj(ctx);
          }
        }(visitors),
        ...);
  };

  explorer(blueprint &data_, environment::tmp &tenv_,
           environment::persist &penv_)
      : data{data_}, tenv{tenv_}, penv{penv_} {}

private:
  void visit(blueprint::base_iterator it, explore::visitor auto &&visitor) {
    if constexpr (eval::visitor<decltype(visitor)>) {
      eval::context ctx{it, tenv, penv};
      visitor(ctx);
    }
  };

private:
  blueprint &data;
  environment::tmp &tenv;
  environment::persist &penv;
};

} // namespace scheme

}; // namespace iuic
