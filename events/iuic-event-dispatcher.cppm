// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:dispatcher;
import iuic.underlying;
import :decl;

export namespace iuic::event {

template <typename T>
concept is_dispatcher = true;

template <const channel &ch> struct dispatcher {
  static_assert(false,
                "For this channel dispatcher is not defined. Pleas watch "
                "docs [iuic.event.dispatcher]");
};

template <const channel &ch> struct trigger {
  trigger(package pkg_) : pkgs{pkg_} {};
  void operator()(auto &&...args) {
    static_assert(is_dispatcher<dispatcher<ch>>,
                  "Invalid dispatcher implimentation. Please watch docs "
                  "[iuic.event.dispatcher]");
    dispatcher<ch>::trigger(pkgs, std::forward<decltype(args)>(args)...);
  };

private:
  package pkgs;
};

template <const channel &ch>
  requires(ch.type == channel::type_e::passive)
struct trigger<ch> {
  static_assert(false, "Passive event's cannot be triggered");
};

template <const channel &ch> struct reader {
  reader(archive a_) : a{a_} {}

  void operator()(auto &&call) {
    static_assert(is_dispatcher<dispatcher<ch>>,
                  "Invalid dispatcher implimentation. Please watch docs "
                  "[iuic.event.dispatcher]");
    dispatcher<ch>::read(a, std::forward<decltype(call)>(call));
  };

private:
  archive a;
};

template <const channel &ch>
  requires(ch.type == channel::type_e::active)
struct reader<ch> {
  static_assert(false, "Passive event's cannot be readed");
};

}; // namespace iuic::event
