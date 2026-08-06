// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:tmp.memory;
import std;
import iuic.underlying;
import :tmp.exception;

namespace iuic::environment {

std::pmr::memory_resource *buffer_overflow_ex_resource() {
  struct : public std::pmr::memory_resource {
  protected:
    void *do_allocate(std::size_t, std::size_t) override {
      throw iuic::exception::tmp_buffer_overflow{};
    };

    void do_deallocate(void *, std::size_t, std::size_t) override {};

    bool do_is_equal(
        const std::pmr::memory_resource &other) const noexcept override {
      return false;
    };
  } static _;

  return &_;
};

struct tmp_memory final : private iuic::advance::interface {
  friend iuic::advance::interface;

  template <typename T> std::span<T> allocate(std::size_t count = 1) {
    return {static_cast<T *>(allocate(sizeof(T), alignof(T), count)), count};
  };

  tmp_memory(iuic::advance::pool &adp) noexcept { rebind(adp); }

public: // BIG-V
  tmp_memory(const tmp_memory &other) = delete;
  tmp_memory(tmp_memory &&other) = delete;
  tmp_memory &operator=(const tmp_memory &other) = delete;
  tmp_memory &operator=(tmp_memory &&other) = delete;
  constexpr ~tmp_memory() {};

private:
  void *allocate(std::size_t size, std::size_t align, std::size_t count = 1) {
    return allocator.allocate(size * count, align);
  };

  void advance() override { allocator.release(); };

private:
  std::array<std::byte, iuic::cenv::num("iuic::env.tmp_buffer_size")
                            .value_or(1024 * 1024 * 4)>
      tmp_buff;
  std::pmr::monotonic_buffer_resource allocator{&tmp_buff, tmp_buff.size(),
                                                buffer_overflow_ex_resource()};
};
} // namespace iuic::environment
