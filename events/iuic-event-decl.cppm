// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:decl;
import std;
import iuic.underlying;

export namespace iuic::event {

template <typename T>
concept is_valide_allocator = true; // use advance::interface && etc

template <typename T>
concept is_valide_pool = true; // use advance::interface && ect

using call = void (*)(erasure::visited::as_mutable packaged_call);

template <typename T>
concept is_base_event_type = requires(T e) {
  { e.type } -> std::same_as<const erasure::type *&>;
  { e.call } -> std::same_as<call &>;
  e.meta;
}; // use advance::interface && ect

struct bus;

struct package {
  erasure::visited::as_mutable meta{nullptr};
  const erasure::type *type{nullptr};
  call call{nullptr};
  // 32 byte
};

namespace policy {
struct thread {
  bool multithread{false};
  // lock-policy
  // etc
};

struct memory {
  std::size_t min_size{512};
  std::size_t max_size{2048};
  bool only_trivially_destructible{true};
  // alloc exception policy
  // etc
};
}; // namespace policy

struct channel {
  policy::thread thread;
  policy::memory memory;
  // etc
};
}; // namespace iuic::event
