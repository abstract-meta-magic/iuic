// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:persist.state;
import std;
import iuic.underlying;
import iuic.state;

namespace iuic::environment {

struct persist_state_storage : iuic::advance::interface {
  using index_t = std::size_t;
  struct slot {
    units::uid uid{0};
    std::uint32_t generation{0};
    std::array<state::value, 3> static_data{nullptr, nullptr, nullptr};
    std::vector<state::value> dynamic_data; // pre alloc 6
  };

  struct proxy {
    proxy(persist_state_storage &storage_, units::uid uid_, index_t i)
        : storage{storage_}, uid{uid_}, access_index{i} {}

    bool has(state::value value) {
      auto &slot = storage.pool[access_index];

      for (auto &state : slot.static_data) {
        if (state == value) {
          return true;
        }
      }

      for (auto &state : slot.dynamic_data) {
        if (state == value) {
          return true;
        }
      }

      return false;
    };

    void attach(state::value value) {
      auto &slot = storage.pool[access_index];

      for (auto &state : slot.static_data) {
        if (state == value) {
          return;
        }
      }

      for (auto &state : slot.static_data) {
        if (state == nullptr) {
          state = value;
          slot.generation = storage.generation;
          return;
        }
      }

      for (auto &state : slot.dynamic_data) {
        if (state == value) {
          return;
        }
      }
      slot.dynamic_data.push_back(value);
      slot.generation = storage.generation;
    };

    void detach(state::value value) {
      auto &slot = storage.pool[access_index];

      for (auto &state : slot.static_data) {
        if (state == value) {
          state = nullptr;
          return;
        }
      }

      auto it =
          std::find(slot.dynamic_data.begin(), slot.dynamic_data.end(), value);
      if (it != slot.dynamic_data.end()) {
        slot.dynamic_data.erase(it);
      }
    };

    void update_lifetime() {
      auto &slot = storage.pool.at(access_index);
      slot.generation = storage.generation;
    };

  private:
    persist_state_storage &storage;
    units::uid uid;
    index_t access_index;
  };

  proxy access(units::uid uid) { return {*this, uid, get_index(uid)}; };

public: // DEBUG API
  template <iuic::utils::is_deduction_context = iuic::utils::deduction_context>
  auto &get_pool()
    requires(iuic::cenv::logic("iuic::debug.api").value_or(false))
  {
    return pool;
  };

  template <iuic::utils::is_deduction_context = iuic::utils::deduction_context>
  auto &get_generation()
    requires(iuic::cenv::logic("iuic::debug.api").value_or(false))
  {
    return generation;
  };

  template <iuic::utils::is_deduction_context = iuic::utils::deduction_context>
  auto get_index_debug(units::uid uid)
    requires(iuic::cenv::logic("iuic::debug.api").value_or(false))
  {
    return get_index(uid);
  };

private:
  static constexpr std::size_t pool_max =
      iuic::cenv::num("iuic::env.persist_max").value_or(8192);

  void sync_GC() {
    // TODO : rework
    for (auto &slot : pool) {
      if (slot.generation != 0) {

        auto diff = static_cast<std::int32_t>(generation - slot.generation);

        if (diff > 6 || diff < -6) {
          std::exchange(slot, {.uid = slot.uid});
        }
      }
    }
  };

  index_t get_index(units::uid uid) {
    auto point = std::to_underlying(uid) & (pool_max - 1);
    auto backup = point;
    auto end = point + 10;

    if (end > pool.max_size()) {
      end = pool.max_size();
    }

    for (; point < end; ++point) {
      if (pool[point].uid == uid) {
        return point;
      } else if (pool[point].uid == units::uid{0}) {
        pool[point].uid = uid;
        return point;
      }
    }

    return backup; // max...
  };

  void advance() override {
    generation += 2;
    generation |= 1;

    sync_GC();
  };

private:
  std::array<slot, pool_max> pool;
  std::uint32_t generation{1};
};
}; // namespace iuic::environment
