// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:dispatcher;
import iuic.underlying;
import :decl;

export namespace iuic::event {
template <const channel &ch> struct dispatcher {
  static void trigger(package pkg, auto &&arg)
    requires(ch.type == channel::type_e::active)
  {
    pkg.call(erasure::visited::as_mutable{arg});
  };

  static void read(archive a, auto &&arg)
    requires(ch.type == channel::type_e::passive)
  {
    // DO JOB later
  };
};

template <const channel &ch> struct trigger {
  trigger(package pkg_) : pkgs{pkg_} {};
  void operator()(auto &&...args) {
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

  void operator()(auto &&...args) {
    dispatcher<ch>::read(a, std::forward<decltype(args)>(args)...);
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
