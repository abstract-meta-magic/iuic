// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:persist.state;
import std;
import iuic.underlying;
import :persist.decl;
import iuic.state;

namespace iuic::environment {

struct persist_state_storage : iuic::advance::interface {
  using index_t = std::size_t;
  struct slot {
    units::uid uid{0};
    std::uint32_t geniration{0};
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
          return;
        }
      }

      for (auto &state : slot.dynamic_data) {
        if (state == value) {
          return;
        }
      }
      slot.dynamic_data.push_back(value);
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
      slot.geniration = storage.geniration;
    };

  private:
    persist_state_storage &storage;
    units::uid uid;
    index_t access_index;
  };

  proxy access(units::uid uid) { return {*this, uid, get_index(uid)}; };

private:
  void sync_GC() {
    // do job
    for (auto &slot : pool) {
      if (slot.geniration != 0 && (geniration - slot.geniration) > 3) {
        std::exchange(slot, {.uid = slot.uid});
      }
    }
  };

  index_t get_index(units::uid uid) {
    auto point = std::to_underlying(uid) & (persist_max * 2 - 1);
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
    ++geniration;
    sync_GC();
  };

private:
  std::array<slot, persist_max * 2> pool;
  std::uint32_t geniration{1};
};
}; // namespace iuic::environment
