// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:allocator;
import iuic.underlying;
import :decl;

export namespace iuic::event {

template <const channel &ch> struct allocator : advance::interface {
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
  std::array<std::byte, ch.memory.size> raw;
  std::pmr::monotonic_buffer_resource alloc__{raw.data(), raw.size(),
                                              std::pmr::null_memory_resource()};
};

}; // namespace iuic::event
