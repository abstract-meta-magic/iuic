// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.symbol;
import std;
import :handle;
import :api.PRIVATE;

namespace /* impl */ {

struct link {
  iuic::symbol::link_handle h;
  std::string_view str;
};

struct slot {
  iuic::symbol::slot_handle h;
  std::string_view str;
  // std::span<link> links; later
};

// 16 byte
struct alignas(16) index_s {
  std::int64_t index;
  union {
    std::uint64_t hash; // ~8 char
    std::uint8_t str[8];
  };
};

// 64 byte
struct alignas(64) index_l {
  static constexpr std::size_t max_size{64};
  std::int64_t index;
  std::int64_t hash;
  std::uint8_t len;
  static constexpr std::size_t str_size{max_size - sizeof(index) - sizeof(len) -
                                        sizeof(hash)};
  std::uint8_t str[str_size];
};

// 128 byte
struct alignas(128) index_ll {
  static constexpr std::size_t max_size{128};
  std::int64_t index;
  std::int64_t hash;
  std::uint8_t len;
  static constexpr std::size_t str_size{max_size - sizeof(index) - sizeof(len) -
                                        sizeof(hash)};
  std::uint8_t str[str_size];
};

// дерево символов\ссылок + index в symbol_data
// для поиска и разрешения
struct scheme {};

struct symbol_location {
  std::int64_t offset;
  std::int64_t size;
};

// плоский набор смещений(на основе scheme) + сами данные
struct symbol_data {
  std::span<const symbol_location> loc;
  const std::uint8_t *data;
};

struct resolution_table {
  std::vector<std::size_t> result; // slot + link
  std::size_t links_offset;
};

struct prediction {
  std::vector<slot> slot;
  std::vector<link> link; // later
};

// prediction - resolution   - scheme
// slot       - index\offset - symbol

struct {
  std::vector<prediction> pre;
} scheme_registry;

}; // namespace

namespace iuic::symbol::api {

scheme_handle reserve_scheme(std::string_view);

slot_handle reserve_slot(scheme_handle, std::string_view);

// return link id
link_handle reserve_slot_link(scheme_handle, slot_handle, std::string_view str);

std::size_t slot_links_count(scheme_handle, slot_handle);

slot_handle resolve_link(scheme_handle, slot_handle, link_handle);

bool is_resolved(scheme_handle);

bool is_resolved(scheme_handle, slot_handle);

bool is_resolved(scheme_handle, slot_handle, link_handle);

// LOCK
lock_handle lock(scheme_handle, slot_handle);

lock_handle lock(lock_handle);

void retire(lock_handle);

bool is_valid_lock(lock_handle);

// TODO : rename ???
bool is_lock_outdated(lock_handle);

std::string_view lock_data(lock_handle);

}; // namespace iuic::symbol::api
