// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.core:event;
import std;
import iuic.underlying;
import iuic.state;
import :environment.persist;
export import :key_code;

export namespace iuic {

namespace event {

struct utils_base {
  utils_base(environment::persist &env_) : env{env_} {}

protected:
  environment::persist &env;
};

struct utils : utils_base {
  utils(environment::persist &env_) : utils_base{env_} {}

  struct : utils_base {
    bool try_visit(units::uid uid,
                   erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto
                       &&visitor) {
      return true;
    };
  } memory{*this};

  struct : utils_base {

  } state{*this};
};
struct event_base {
  utils utils;
  units::uid object; // can be null
  units::uid uid;
};

namespace global {
struct key : event_base {
  key_code code;
  units::ui::position current_mouse_position{};
};

struct pointer : event_base {
  units::ui::position current_mouse_position{};
  units::ui::position old_mouse_position{};
};

}; // namespace global

namespace local {
struct key : event_base {
  key_code code;
  units::ui::position current_mouse_position{};
  units::ui::rect rect;
};

struct pointer : event_base {
  units::ui::position current_mouse_position{};
  units::ui::position old_mouse_position{};
  units::ui::rect rect;
};

}; // namespace local
} // namespace event
}; // namespace iuic

namespace iuic::event {

// pointer
using global_pointer_move_event_fpt = void (*)(event::global::pointer);
using local_pointer_move_event_fpt = void (*)(event::local::pointer);

// key
using global_key_event_fpt = void (*)(event::global::key);
using local_key_event_fpt = void (*)(event::local::key);

using variadic_callback =
    std::variant<local_key_event_fpt, local_pointer_move_event_fpt,
                 global_key_event_fpt, global_pointer_move_event_fpt>;

template <typename T>
concept callback_cpt = requires(T &&call) { variadic_callback{call}; };

struct value {
  /*
    Why not std::variant?
    Becaose aligned. 32
  */
  union {
    local_key_event_fpt lk;
    local_pointer_move_event_fpt lpm;
    global_key_event_fpt gk;
    global_pointer_move_event_fpt gpm;
  };
  units::uid object;
  units::uid uid;
  enum meta_e { triggered = 0, local, key, pointer };
  using meta_t = std::bitset<4>;
  meta_t meta;
};

}; // namespace iuic::event
