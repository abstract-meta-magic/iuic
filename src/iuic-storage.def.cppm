

module;

#include <functional>

export module iuic.core:storage.def;
import :hash;

export namespace iuic {

struct object_registry_key {
  constexpr object_registry_key() noexcept = default;
  constexpr object_registry_key(hash::hash_t val) noexcept : value{val} {};

  constexpr operator hash::hash_t &() noexcept { return value; };
  constexpr operator const hash::hash_t &() const noexcept { return value; };
  constexpr operator hash::hash_t() const noexcept { return value; };
  constexpr auto
  operator<=>(const object_registry_key &) const noexcept = default;

  hash::hash_t value;
};

struct text_registry_key {
  constexpr text_registry_key() noexcept = default;
  constexpr text_registry_key(hash::hash_t val) noexcept : value{val} {};

  constexpr operator hash::hash_t &() noexcept { return value; };
  constexpr operator const hash::hash_t &() const noexcept { return value; };
  constexpr operator hash::hash_t() const noexcept { return value; };
  constexpr auto
  operator<=>(const text_registry_key &) const noexcept = default;

  hash::hash_t value;
};

using ork_t = object_registry_key;
using trk_t = text_registry_key;

struct object_storage;
struct text_storage;

}; // namespace iuic

namespace std {
template <> struct hash<iuic::ork_t> {
  size_t operator()(const iuic::ork_t &ork) const { return ork.value; };
};

template <> struct hash<iuic::trk_t> {
  size_t operator()(const iuic::trk_t &trk) const { return trk.value; };
};
}; // namespace std
