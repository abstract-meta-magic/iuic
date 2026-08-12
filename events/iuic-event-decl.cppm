// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:decl;
import std;
import iuic.underlying;

export namespace iuic::event {

template <typename... ARGS> struct meta_call {
  erasure::visited::as_mutable call;
  std::tuple<ARGS...> args;
};

template <typename T>
concept is_valide_allocator =
    std::is_base_of_v<T, iuic::advance::interface> &&
    requires(T &alloc, std::size_t bytes, std::size_t align) {
      alloc.allocate(bytes, align);
      // don't use alloc.deallocate because
      // frame system with advance::pool
    };

template <typename T>
concept is_valide_pool = std::is_base_of_v<T, iuic::advance::interface>;

using call = void (*)(erasure::visited::as_mutable packaged_call);

template <typename T>
concept is_base_event_type = requires(T e) {
  { e.type } -> std::same_as<const erasure::type *&>;
  { e.call } -> std::same_as<call &>;
};

// for active events(trigger)
struct package /* align ?? */ {
  erasure::visited::as_mutable meta{nullptr};
  const erasure::type *type{nullptr};
  call call{nullptr};
  // 32 byte
};

// for passive events(read)
struct archive /* align ?? */ {
  erasure::visited::as_mutable data{nullptr};
  const erasure::type *type{nullptr};
};

namespace policy {

struct thread {
  bool multithread{false};
};

struct memory {
  std::size_t size{1024 * 8};
  bool only_trivially_destructible{true};
  // alloc exception policy
  // etc
};

struct emmiter_provide {
  enum class status { none, front, back };
  status factory{status::none};
  status packanger{status::none};
};
}; // namespace policy

struct channel {
  // TODO : in alpha or beta
  enum class type_e {
    active,  // событие ожидает .trigger(..)
    passive, // события является отправленным. Просто данные в пуле
  } type{channel::type_e::active};
  policy::thread thread;
  policy::memory memory;
  policy::emmiter_provide emmiter_provide;
  // etc
};

}; // namespace iuic::event
