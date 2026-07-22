// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.env:persist.object;
import std;
import iuic.underlying;
import :persist.decl;

namespace iuic::environment {

export enum object_state {
  non_exist,
  deleted,
  outdated_this_type,
  outdated_other_type,
  alive_this_type,
  alive_other_type,
  reserve_this_type,
  reserve_other_type,
  reserve_undefined_type,
};

struct arena_id {
  constexpr arena_id(std::uint64_t value) : raw{value} {};
  static constexpr std::uint64_t allocated_state = 0;   // in : 2 00
  static constexpr std::uint64_t deallocated_state = 3; // in : 2 11

  static constexpr arena_id deallocated() {
    return {std::numeric_limits<std::uint64_t>::max()};
  };

  union {
    struct {
      std::uint64_t pool : 8;
      std::uint64_t chunk : 8;
      std::uint64_t index : 46;
      std::uint64_t allocation_state__ : 2; // for invalidate
    } location;
    std::uint64_t raw;
  };

  bool is_allocated() {
    return location.allocation_state__ == allocated_state;
  };

  void set_allocated() { location.allocation_state__ = allocated_state; };

  void set_deallocated() { location.allocation_state__ = deallocated_state; };

  auto operator<=>(const arena_id &other) const noexcept {
    return raw <=> other.raw;
  };

  bool operator==(const arena_id &other) const noexcept {
    return raw == other.raw;
  };

  bool operator!=(const arena_id &other) const noexcept {
    return not(*this == other);
  };
};

// lifetime size_t::max == immortal

struct persist_object_storage_arena {
  // sized 16 32 64 128 256
  struct alignas(64) chunk {
    static constexpr std::size_t native_size = 64 * 1024;
    std::byte *memory{nullptr};
    std::size_t capacity{0};
    std::size_t slot_size{0};
    std::uint32_t free_count{0};
    std::uint32_t free_list_head{0};
    // -----
    std::uint32_t next_chunk_index{0};
    std::uint32_t creation_gen{0};
    enum {
      active,
      freeze,
      free,
    } state{free};
  };

  static_assert(sizeof(chunk) == 64, "Chunk size must be 64 bytes");

  void *get(arena_id id) {
    if (not id.is_allocated()) {
      return nullptr;
    }

    // TODO : #DEBUG CODE

    auto &chunk = sized_pool[id.location.pool].chunks[id.location.chunk];

    if (chunk.state != chunk::free) {
      return chunk.memory + (id.location.index * chunk.slot_size);
    }

    return nullptr;
  };

  arena_id allocate(std::size_t size) {
    std::size_t index_pool{sized_pool.max_size()};

    arena_id result = arena_id::deallocated();

    if (size < 17) {
      result.location.pool = 0;
      allocate__(sized_pool[0], result);
    } else if (size < 33) {
      result.location.pool = 1;
      allocate__(sized_pool[1], result);
    } else if (size < 65) {
      result.location.pool = 2;
      allocate__(sized_pool[2], result);
    } else if (size < 129) {
      result.location.pool = 3;
      allocate__(sized_pool[3], result);
    } else if (size < 257) {
      result.location.pool = 4;
      allocate__(sized_pool[4], result);
    } else {
      // large object
    }

    return result;
  };

  void deallocate(arena_id id) {
    if (id.is_allocated()) {
      // in DEBUG CHECK BOUNDS
      deallocate__(sized_pool[id.location.pool], id);
    }
  };

  struct sized_chunk_pool {
    std::array<chunk, 64> chunks;
    std::size_t free_head{0};
    std::size_t slot_size{0};
  };

public: // BIG-V
  persist_object_storage_arena() {
    std::uint32_t slot_size = 16;

    for (std::uint32_t i{0}; i < sized_pool.max_size(); ++i) {
      sized_pool[i].slot_size = slot_size;

      fill_chunks_info(sized_pool[i]);

      slot_size *= 2;
    }
  };

  void fill_chunks_info(sized_chunk_pool &pool) {
    for (auto &chunk : pool.chunks) {
      chunk.slot_size = pool.slot_size;
      chunk.capacity = chunk::native_size / chunk.slot_size;
      chunk.free_count = chunk.capacity;
      chunk.free_list_head = 0;
      chunk.state = chunk::free;
    }
  };

  persist_object_storage_arena(const persist_object_storage_arena &) = delete;
  persist_object_storage_arena(persist_object_storage_arena &&) = delete;
  persist_object_storage_arena &
  operator=(const persist_object_storage_arena &) = delete;
  persist_object_storage_arena &
  operator=(persist_object_storage_arena &&) = delete;

  ~persist_object_storage_arena() {};

private:
  void deallocate__(sized_chunk_pool &pool, arena_id &id) {
    deallocate_slot(pool.chunks[id.location.chunk], id);
    // check for memory free
  };

  void deallocate_slot(chunk &chunk, arena_id &id) {
    std::uint32_t slot = id.location.index;

    *reinterpret_cast<std::uint32_t *>(
        chunk.memory + (slot * chunk.slot_size)) = chunk.free_list_head;

    chunk.free_list_head = slot;
    ++chunk.free_count;
  };

  void allocate__(sized_chunk_pool &pool, arena_id &id) {
    if (chunk *ch = find_chunk(pool, id)) {
      allocate_slot(*ch, id);
    } else if (ch = allocate_chunk(pool, id); ch) {
      allocate_slot(*ch, id);
    }
  };

  chunk *find_chunk(sized_chunk_pool &pool, arena_id &id) {
    for (std::uint32_t chunk_index{0}; chunk_index < pool.chunks.max_size();
         ++chunk_index) {
      auto &chunk = pool.chunks[chunk_index];
      if (chunk.state == chunk::active && chunk.free_count > 0) {
        id.location.chunk = chunk_index;
        return &chunk;
      }
    }

    return nullptr;
  };

  chunk *allocate_chunk(sized_chunk_pool &pool, arena_id &id) {
    for (std::uint32_t chunk_index{0}; chunk_index < pool.chunks.max_size();
         ++chunk_index) {
      auto &chunk = pool.chunks[chunk_index];

      if (chunk.state == chunk::free) {
        chunk.memory = new std::byte[chunk.native_size];

        std::uint32_t *slot_ptr =
            reinterpret_cast<std::uint32_t *>(chunk.memory);

        auto end = chunk.capacity - 1;
        for (std::uint32_t i{0}; i < end; ++i) {
          *slot_ptr = i + 1;
          slot_ptr = reinterpret_cast<std::uint32_t *>(
              chunk.memory + (i + 1) * chunk.slot_size);
        }
        *slot_ptr = std::numeric_limits<std::uint32_t>::max();

        chunk.free_list_head = 0;
        chunk.state = chunk::active;

        id.location.chunk = chunk_index;

        return &chunk;
      }
    }

    return nullptr;
  };

  void allocate_slot(chunk &chunk, arena_id &id) {
    std::uint32_t slot = chunk.free_list_head;

    /* Later in #DEBUG MODE
    if (chunk.free_list_head == std::numeric_limits<std::uint32_t>::max()) {
      return;
    }
    */

    chunk.free_list_head = *reinterpret_cast<std::uint32_t *>(
        chunk.memory + (slot * chunk.slot_size));
    --chunk.free_count;

    id.location.index = slot;
    id.set_allocated();
  };

private:
  std::array<sized_chunk_pool, 5> sized_pool;
};

struct persist_object_storage : iuic::advance::interface {
  using index_t = std::size_t;

  struct slot {
    units::uid uid{0};
    arena_id id{arena_id::deallocated()};
    const erasure::type *type{erasure::type::none()};
    std::uint32_t lifetime{0}; // max == immortal
    // gen [< -\+ >] world.gen <= lifetime == object dead
    std::uint32_t generation{0};
  };

  // alloc,nonexist,etc

  struct proxy {
    proxy(persist_object_storage &storage_, units::uid uid_, index_t index_,
          const erasure::type *type_)
        : storage{storage_}, uid{uid_}, access_index{index_},
          required_type{type_} {};

    object_state state() {
      auto &slot = storage.pool[access_index];
      if (slot.uid == uid) {
        if (slot.type == erasure::type::none()) {
          return non_exist;
        }

        if (slot.type == required_type) {
          if (not is_alive()) {
            return object_state::outdated_this_type;
          } else if (slot.id.is_allocated()) {
            return object_state::alive_this_type;
          } else {
            return object_state::reserve_this_type;
          }
        } else {
          if (not is_alive()) {
            return object_state::outdated_other_type;
          } else if (slot.id.is_allocated()) {
            return object_state::alive_other_type;
          } else {
            return object_state::reserve_other_type;
          }
        }
      }

      return object_state::non_exist;
    };

    void reserve() {
      auto &slot = storage.pool[access_index];

      if (slot.type == erasure::type::none()) {
        slot.type = required_type;
        slot.lifetime = required_type->livetime;
        slot.generation = storage.generation;
        slot.uid = uid;
      } else if (not is_alive()) {
        slot.type = required_type;
        slot.lifetime = required_type->livetime;
        slot.generation = storage.generation;
        destruct();
        deallocate();
      }
    };

    void free() {
      auto &slot = storage.pool[access_index];

      auto garbage = std::exchange(slot, {.uid = slot.uid});
      if (garbage.id.is_allocated()) {
        garbage.type->dtor(storage.arena.get(garbage.id));
        storage.arena.deallocate(garbage.id);
      }
    };

    void construct(std::invocable<void *> auto &&ctor) {
      auto &slot = storage.pool[access_index];
      // возможно еще проверки на валидность
      if (slot.id.is_allocated() && slot.type == required_type) {
        ctor(storage.arena.get(slot.id));
      }
    };

    void destruct() {
      auto &slot = storage.pool[access_index];

      if (slot.type != erasure::type::none() && slot.id.is_allocated()) {
        slot.type->dtor(storage.arena.get(slot.id));
      }
    };

    void deallocate() {
      auto &slot = storage.pool[access_index];

      if (slot.id.is_allocated()) {
        storage.arena.deallocate(slot.id);
        slot.id = arena_id::deallocated();
      }
    };

    void allocate() {
      auto &slot = storage.pool[access_index];

      if (not slot.id.is_allocated() && slot.type != erasure::type::none()) {
        slot.id = storage.arena.allocate(slot.type->size);
      }
    };

    erasure::visited get() {
      auto &slot = storage.pool[access_index];
      if (slot.id.is_allocated() && slot.type != erasure::type::none() &&
          is_alive()) {
        return erasure::visited{storage.arena.get(slot.id), slot.type};
      } else {
        return nullptr;
      }
    };

    void update_lifetime() {
      auto &slot = storage.pool[access_index];

      if (slot.lifetime == 0)
        return;

      if (is_alive() && required_type == slot.type) {
        slot.generation = storage.generation;
      }
    };

  private:
    bool is_alive() {
      auto &slot = storage.pool[access_index];
      if (slot.lifetime == 0)
        return true;

      auto dif = storage.generation - slot.generation;

      return dif < slot.lifetime;
    };

  private:
    persist_object_storage &storage;
    index_t access_index;
    units::uid uid;
    const erasure::type *required_type;
  };

  proxy access(units::uid uid, const erasure::type *t) {
    return proxy{*this, uid, get_index(uid), t};
  };

  erasure::visited get(units::uid uid) {
    auto &slot = pool[get_index(uid)];

    if (slot.id.is_allocated()) {
      return {arena.get(slot.id), slot.type};
    } else {
      return nullptr;
    }
  };

private:
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
        return point;
      }
    }

    return backup; // max...
  }

  void sync_clean() {
    for (std::size_t i = 0; i < pool.size(); ++i) {
      auto &slot = pool[i];

      if (slot.uid == units::uid{0} || !slot.id.is_allocated()) {
        continue;
      }

      if (!is_slot_alive(slot)) {
        if (slot.type != erasure::type::none()) {
          void *ptr = arena.get(slot.id);
          if (ptr) {
            slot.type->dtor(ptr);
          }
        }

        arena.deallocate(slot.id);

        std::exchange(slot, {.uid = slot.uid});
      }
    }
  }

  // Вспомогательный метод для проверки (можно сделать private методом класса)
  bool is_slot_alive(const slot &s) const {
    if (s.lifetime == 0)
      return true; // Immortal

    return (generation - s.generation) < s.lifetime;
  }

  // later for async GC
  // bool run_marker();
  // void touch_GC();
private: // ADVANCE
  void advance() override {
    ++generation;
    sync_clean();
  };

private:
  std::array<slot, persist_max * 2> pool; // x 2 for <40%
  persist_object_storage_arena arena;
  std::uint32_t generation; // on advance ++
  // кол-во разбиений для вызова маркера.
  // [Later] thread for dead_marker and GC ???
};

void reserve(persist_object_storage &s, units::uid uid,
             const erasure::type *t) {
  if (auto access = s.access(uid, t);
      access.state() == object_state::non_exist) {
    access.reserve();
  } else if (access.state() == object_state::reserve_this_type) {
    access.update_lifetime();
  };
};

void init_if_not(persist_object_storage &s, units::uid uid,
                 const erasure::type *t) {

  if (auto access = s.access(uid, t);
      access.state() == object_state::reserve_this_type) {
    access.allocate();
    access.construct([](void *ptr) { new (ptr) int{4}; });
  } else if (access.state() == object_state::alive_this_type) {
    access.update_lifetime();
  };

  // SYNC GC IN ADVANCE::POOL
};

}; // namespace iuic::environment
