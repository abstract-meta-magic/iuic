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
      return env.object.get(uid).try_visit(
          std::forward<decltype(visitor)>(visitor));
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

// TODO : IMPL
// можно иметь глобальное состояние
// и проверять его.
// b.state.global.has<state::error>(); как приме
struct attach_state : event_base {
  state::value state;
};
// TODO : IMPL
struct detach_state : event_base {
  state::value state;
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

// TODO : IMPL
struct attach_state : event_base {
  state::value state;
};
// TODO : IMPL
struct detach_state : event_base {
  state::value state;
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
  value(local_key_event_fpt lk_, units::uid uid_, units::uid object_)
      : lk{lk_}, uid{uid_}, object{object_} {
    meta.set(local);
    meta.set(key);
  };

  value(local_pointer_move_event_fpt lpm_, units::uid uid_, units::uid object_)
      : lpm{lpm_}, uid{uid_}, object{object_} {
    meta.set(local);
    meta.set(pointer);
  };

  value(global_key_event_fpt gk_, units::uid uid_, units::uid object_)
      : gk{gk_}, uid{uid_}, object{object_} {
    meta.set(key);
  };

  value(global_pointer_move_event_fpt gpm_, units::uid uid_, units::uid object_)
      : gpm{gpm_}, uid{uid_}, object{object_} {
    meta.set(pointer);
  };

  bool is_local() const { return meta.test(local); };

  bool is_global() const { return not meta.test(local); };

  bool is_key_event() const { return meta.test(key); };

  bool is_pointer_event() const { return meta.test(pointer); };

  bool is_triggered() const { return meta.test(triggered); };

  const void *callback_address() const { return reinterpret_cast<void *>(lk); };

  const units::uid uid;

private:
  friend void trigger(value &, environment::persist &);
  units::uid object;
  union {
    local_key_event_fpt lk;
    local_pointer_move_event_fpt lpm;
    global_key_event_fpt gk;
    global_pointer_move_event_fpt gpm;
  };
  enum meta_e { triggered = 0, local, key, pointer };
  using meta_t = std::bitset<4>;
  meta_t meta;
};

}; // namespace iuic::event

namespace iuic::event {
void trigger(value &e, environment::persist &penv) {
  switch (e.meta.to_ulong()) {
  case 1 << value::key | 1 << value::local: {
    e.lk({{.utils = utils{penv},
           .object = units::uid{e.object},
           .uid = units::uid{e.uid}},
          .code = penv.external.key_code});
    e.meta.set(value::triggered);
    break;
  }
  case 1 << value::key: {
    e.gk({penv, .code = penv.external.key_code});
    e.meta.set(value::triggered);
    break;
  }
  case 1 << value::pointer | 1 << value::local: {
    e.lpm({penv});
    e.meta.set(value::triggered);
    break;
  }
  case 1 << value::pointer: {
    e.gpm({penv});
    e.meta.set(value::triggered);
    break;
  }
  }
};
}; // namespace iuic::event
