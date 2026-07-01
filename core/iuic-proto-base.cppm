// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:proto.base;
import std;
import iuic.underlying;
import iuic.event;
import iuic.env;
import iuic.state;

namespace iuic::proto::base {

// base define state
namespace state {
export constexpr iuic::state::decl hovered{
    iuic::state::decl::instance_of<hovered>()};

export constexpr iuic::state::decl local{
    iuic::state::decl::instance_of<hovered>()};

export constexpr iuic::state::decl active{
    iuic::state::decl::instance_of<hovered>()};
}; // namespace state

namespace capabilities {
export constexpr iuic::external::capabilities::decl scissor;

export constexpr iuic::external::capabilities::decl color;
}; // namespace capabilities

namespace event {

export constexpr iuic::event::channel local;
export constexpr iuic::event::channel global;
export constexpr iuic::event::channel global_state;
constexpr iuic::event::channel global_cmd;

struct utils {
  utils(iuic::environment::persist *penv_, iuic::environment::tmp *tenv_,
        units::uid owner_, units::uid obj_)
      : penv{penv_}, tenv{tenv_}, owner{owner_}, obj{obj_} {};

  void try_visit_object(auto &&call) {
    penv->object.get(obj).try_visit(std::forward<decltype(call)>(call));
  };

  struct : iuic::utils::member_for<utils> {

  } memory{*this};

  struct : iuic::utils::member_for<utils> {
    bool has(iuic::state::value value) {
      return self().penv->state.has(self().owner, value);
    };

    void attach(iuic::state::value value) {
      self().penv->state.attach(self().owner, value);
    };
    void detach(iuic::state::value value) {
      self().penv->state.detach(self().owner, value);
    };
  } state{*this};

  struct : iuic::utils::member_for<utils> {

    template <const iuic::event::channel &ch> void emit(auto &&) {};
  } event{*this};

private:
  iuic::environment::persist *penv{nullptr};
  iuic::environment::tmp *tenv{nullptr};
  units::uid owner;
  units::uid obj;
};

struct pkg_meta {
  units::uid owner;
  units::uid obj;
  erasure::visited::as_mutable meta{nullptr};
};

struct meta_call {
  erasure::visited::as_mutable call;
  erasure::visited::as_mutable arg;
};

export struct key {
  utils utils;
  units::keycode key;
};
export struct pointer {
  utils utils;
  units::ui::position position;
  // old\current ???
};

export struct state {
  utils utils;
  enum { attach, detach } type;
  iuic::state::value value;
};

template <typename T> struct pkg_source {
  struct pkg_meta meta;
  const erasure::type *type;
  T call;
};
}; // namespace event

export namespace query {

namespace tag {
struct element : iuic::query::tag {};
struct event_local : iuic::query::tag {};
struct event_global : iuic::query::tag {};

template <const iuic::event::channel &channel__> struct event {
  static constexpr auto &channel = channel__;
};

}; // namespace tag

constexpr iuic::query::expr<tag::event_global> event_global{};
constexpr iuic::query::expr<tag::event_local> event_local{};
constexpr iuic::query::expr<tag::element> element{};
template <const iuic::event::channel &channel__>
constexpr iuic::query::expr<tag::event<channel__>> event{};

struct hit {
  using type_tag = iuic::query::type;
  units::ui::position point;
};

struct has_state {
  using type_tag = iuic::query::type;
  iuic::state::value state;
};

struct valid_t {
  using type_tag = iuic::query::type;
} constexpr valid;

constexpr has_state hovered{state::hovered};
}; // namespace query
}; // namespace iuic::proto::base

export namespace iuic::event {

template <> struct packager<iuic::proto::base::event::local> {
  // DO JOB
  template <typename T>
  package make(iuic::proto::base::event::pkg_source<T> src,
               allocator<iuic::proto::base::event::local> &alloc) {
    // DO PACK
    using namespace iuic::proto::base::event;
    package pkg;

    // alloc in tpm
    auto meta_call = [call = src.call](erasure::visited::as_mutable arg) {
      if constexpr (requires(iuic::proto::base::event::key &e) { call(e); }) {
        arg.try_visit([&](iuic::proto::base::event::key &e) { call(e); });
      } else {
        arg.try_visit([&](iuic::proto::base::event::pointer &e) { call(e); });
      }
    };

    src.meta.meta = meta_call;
    // set meta
    pkg.meta = new (alloc.allocate<pkg_meta>()) pkg_meta{src.meta};

    // set type
    pkg.type = src.type;

    // make call
    pkg.call = [](erasure::visited::as_mutable meta) static {
      // WARN
      meta.unsafe_visit([](iuic::proto::base::event::meta_call &src) {
        // WARN
        src.call.unsafe_visit(
            [&](decltype(meta_call) &call) { call(src.arg); });
      });
    };

    return pkg;
  };
};

template <> struct dispatcher<iuic::proto::base::event::local> {
  static void trigger(package pkg, units::keycode key,
                      environment::persist *penv, environment::tmp *tenv) {

    // WARN
    pkg.meta.unsafe_visit([&](iuic::proto::base::event::pkg_meta &meta) {
      iuic::proto::base::event::key e{.utils =
                                          iuic::proto::base::event::utils{
                                              penv,
                                              tenv,
                                              meta.owner,
                                              meta.obj,
                                          },
                                      .key = key};

      iuic::proto::base::event::meta_call meta_call{
          .call = meta.meta,
          .arg = e,
      };

      pkg.call(erasure::visited::as_mutable{meta_call});
    });
  };

  static void trigger(package pkg, units::ui::position pointer,
                      environment::persist *penv, environment::tmp *tenv) {

    // WARN
    pkg.meta.unsafe_visit([&](iuic::proto::base::event::pkg_meta &meta) {
      iuic::proto::base::event::pointer e{.utils =
                                              iuic::proto::base::event::utils{
                                                  penv,
                                                  tenv,
                                                  meta.owner,
                                                  meta.obj,
                                              },
                                          .position = pointer};

      iuic::proto::base::event::meta_call meta_call{
          .call = meta.meta,
          .arg = e,
      };

      pkg.call(erasure::visited::as_mutable{meta_call});
    });
  };
};

template <> struct packager<iuic::proto::base::event::global> {
  // DO JOB
  template <typename T>
  package make(iuic::proto::base::event::pkg_source<T> src,
               allocator<iuic::proto::base::event::global> &alloc) {
    // DO PACK
    using namespace iuic::proto::base::event;
    package pkg;

    // alloc in tpm
    auto meta_call = [call = src.call](erasure::visited::as_mutable arg) {
      if constexpr (requires(iuic::proto::base::event::key &e) { call(e); }) {
        arg.try_visit([&](iuic::proto::base::event::key &e) { call(e); });
      } else {
        arg.try_visit([&](iuic::proto::base::event::pointer &e) { call(e); });
      }
    };

    src.meta.meta = meta_call;
    // set meta
    pkg.meta = new (alloc.allocate<pkg_meta>()) pkg_meta{src.meta};

    // set type
    pkg.type = src.type;

    // make call
    pkg.call = [](erasure::visited::as_mutable meta) static {
      // WARN
      meta.unsafe_visit([](iuic::proto::base::event::meta_call &src) {
        // WARN
        src.call.unsafe_visit(
            [&](decltype(meta_call) &call) { call(src.arg); });
      });
    };

    return pkg;
  };
};

template <> struct dispatcher<iuic::proto::base::event::global> {
  static void trigger(package pkg, units::keycode key,
                      environment::persist *penv, environment::tmp *tenv) {

    // WARN
    pkg.meta.unsafe_visit([&](iuic::proto::base::event::pkg_meta &meta) {
      iuic::proto::base::event::key e{.utils =
                                          iuic::proto::base::event::utils{
                                              penv,
                                              tenv,
                                              meta.owner,
                                              meta.obj,
                                          },
                                      .key = key};

      iuic::proto::base::event::meta_call meta_call{
          .call = meta.meta,
          .arg = e,
      };

      pkg.call(erasure::visited::as_mutable{meta_call});
    });
  };

  static void trigger(package pkg, units::ui::position pointer,
                      environment::persist *penv, environment::tmp *tenv) {

    // WARN
    pkg.meta.unsafe_visit([&](iuic::proto::base::event::pkg_meta &meta) {
      iuic::proto::base::event::pointer e{.utils =
                                              iuic::proto::base::event::utils{
                                                  penv,
                                                  tenv,
                                                  meta.owner,
                                                  meta.obj,
                                              },
                                          .position = pointer};

      iuic::proto::base::event::meta_call meta_call{
          .call = meta.meta,
          .arg = e,
      };

      pkg.call(erasure::visited::as_mutable{meta_call});
    });
  };
};
}; // namespace iuic::event

namespace iuic::proto::advanced {
namespace event {

export struct quick_sound {
  std::string_view name;
  float volume;
};

export constexpr iuic::event::channel sound{
    .type = iuic::event::channel::type_e::passive};
}; // namespace event
}; // namespace iuic::proto::advanced
