// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:allocator;
import iuic.underlying;
import :decl;

export namespace iuic::event {

template <channel ch> struct allocator : advance::interface {
  static_assert(ch.memory.only_trivially_destructible,
                "support only trivially destructible allocation");

  allocator(advance::pool &p) { rebind(p); };

  template <erasure::is_pure_type T>
    requires std::is_trivially_destructible_v<T>
  T *allocate() {
    return static_cast<T *>(alloc__.allocate(sizeof(T), alignof(T)));
  };

private:
  void advance() override { alloc__.release(); };

private:
  std::byte raw[ch.memory.max_size];
  std::pmr::monotonic_buffer_resource alloc__{raw};
};

}; // namespace iuic::event
