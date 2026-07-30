// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module;

namespace iuic::environment {

#ifdef IUIC_TMP_STATIC_POILCY_STORAGE

constexpr bool policy_tmp_storage_use_static{true};

#ifndef IUIC_TMP_STATIC_POILCY_STORAGE_MAX
constexpr std::size_t policy_tmp_storage_max{4048};
#else
constexpr std::size_t policy_tmp_storage_max{
    IUIC_TMP_STATIC_POILCY_STORAGE_MAX};
#endif

#else
constexpr bool policy_tmp_storage_use_static{false};
#endif
}; // namespace iuic::environment

export module iuic.env:tmp.policy;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;

namespace iuic::environment {

template <typename T>
concept is_policy = std::is_enum_v<T> && sizeof(T) <= sizeof(std::uint64_t) &&
                    requires(T value) {
                      T::unset;
                      value = T::unset;
                      static_cast<std::uint64_t>(value);
                      requires std::to_underlying(T::unset) == 0;
                    };

inline std::size_t hash(units::uid uid, const erasure::type *type) {
  std::size_t lhs{std::to_underlying(uid)};
  std::size_t rhs{reinterpret_cast<std::uintptr_t>(type)};

  if constexpr (sizeof(std::size_t) == 8) {
    lhs ^= (rhs * 0x9E3779B97F4A7C15ULL);

    lhs ^= lhs >> 33;
    lhs *= 0xFF51AFD7ED558CCDULL;
    lhs ^= lhs >> 33;
    lhs *= 0xC4CEB9FE1A85EC53ULL;
    lhs ^= lhs >> 33;
    return lhs;
  } else if constexpr (sizeof(std::size_t) == 4) {
    lhs ^= (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));

    lhs ^= lhs >> 16;
    lhs *= 0x45d9f3bULL;
    lhs ^= lhs >> 16;
    return lhs;
  } else {
    throw std::logic_error{"unsupported system"};
  }
};

// tmp
struct policy_storage_dynamic__ : iuic::advance::interface {
  static constexpr std::size_t init_size{1024};
  struct slot {
    units::uid uid{0};
    const erasure::type *type{nullptr};
    std::uint64_t value{0};
  };

  template <is_policy T> void set(units::uid uid, T value) {
    if (auto slot_index = find_slot(uid, erasure::type::from<T>());
        slot_index != invalid_index) {
      slots[slot_index].value = static_cast<std::uint64_t>(value);
    }
  };

  template <is_policy T> T get(units::uid uid) {
    if (auto slot_index = find_slot(uid, erasure::type::from<T>());
        slot_index != invalid_index) {
      return static_cast<T>(slots[slot_index].value);
    } else {
      return T::unset;
    }
  };

public: // ctor
  policy_storage_dynamic__() {
    slots = std::vector<slot>(init_size); // 1024
    size = init_size;
    mask = init_size - 1;
  };

private: // advance
  void advance() override {
    std::memset(slots.data(), 0, size * sizeof(struct slot));
    load = 0;
  };

private:
  static constexpr std::size_t invalid_index =
      std::numeric_limits<std::size_t>::max();

  template <bool check = true>
  std::size_t find_slot(units::uid uid, const erasure::type *type) {
    std::size_t h = hash(uid, type);
    std::size_t slot_index = h & mask;

    if constexpr (check) {
      check_fill();
    }

    for (;;) {
      if (auto &slot = slots[slot_index];
          slot.type == type && slot.uid == uid) {
        return slot_index;
      } else if (slot.type == nullptr) {
        slot.type = type;
        slot.uid = uid;
        ++load;
        return slot_index;
      }
      slot_index = (slot_index + 1) & mask;
    }
  };

  void rehash() {
    size <<= 1;
    mask = size - 1;
    load = 0;

    std::vector<slot> old_slots(size);
    std::swap(old_slots, slots);

    for (auto &slot : old_slots) {
      if (slot.type != nullptr) {
        std::size_t insert_index = find_slot<false>(slot.uid, slot.type);
        slots[insert_index].value = slot.value;
      }
    }
  };

  void check_fill() {
    if (load * 10 > size * 7) {
      rehash();
    }
  };

private:
  std::size_t mask;
  std::size_t size;
  std::size_t load;
  std::vector<slot> slots;
};

template <bool = policy_tmp_storage_use_static>
struct policy_storage_base_resolution;

template <> struct policy_storage_base_resolution<true> {
  // TODO : impl static std::array pool
  using type = policy_storage_dynamic__;
};

template <> struct policy_storage_base_resolution<false> {
  using type = policy_storage_dynamic__;
};

struct policy_storage : public policy_storage_base_resolution<>::type {};
}; // namespace iuic::environment
