// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.shared;
import std;
import :scheme.base;

namespace iuic::scheme {

export struct partition : base {
  // API
  partition(base &b, units::uid owner_) : base{b}, owner{owner_} {}

  struct : utils::member_for<partition> {

    bool has(state::value v) {
      return self().penv->state.access(self().owner).has(v);
    };

    void attach(state::value v) {
      self().penv->state.access(self().owner).attach(v);
    };
    void detach(state::value v) {
      self().penv->state.access(self().owner).detach(v);
    };

  } state{*this};

  // owner_state_attach();
  // object_memory_try_visit();
  // operator=(units::uid owner_)

private:
  units::uid owner;
};

export struct capture : base {
  // API
  capture(base &b, units::uid owner__, units::uid obj__)
      : base{b}, owner_{owner__}, obj_{obj__} {}

  struct : utils::member_for<capture> {
    struct : utils::member_for<capture> {
      bool has(state::value v) {
        return self().penv->state.access(self().owner_).has(v);
      };

      void attach(state::value v) {
        self().penv->state.access(self().owner_).attach(v);
      };
      void detach(state::value v) {
        self().penv->state.access(self().owner_).detach(v);
      };
    } state{self()};

    struct : utils::member_for<capture> {

    } memory{self()};

    struct : utils::member_for<capture> {
      // emit ??
      // only archive(passive) ???
      template <const event::channel &ch>
        requires(ch.type == event::channel::type_e::passive)
      void emit(auto &&data) {
        self().tenv->event.emit<ch>(std::forward<decltype(data)>(data));
      }
    } event{self()};
  } owner{*this};

  struct : utils::member_for<capture> {
    struct : utils::member_for<capture> {
      void try_visit(auto &&call) {
        erasure::visited::as_mutable{self().penv->object.get(self().obj_)}
            .try_visit(std::forward<decltype(call)>(call));
      };
    } memory{self()};
  } obj{*this};

  // operator=(std::tuple<units::uid,units::uid> owner_)
  // v = {owner,obj};
private:
  units::uid owner_;
  units::uid obj_;
};

} // namespace iuic::scheme
