export module iuic.core:storage.def;
import std;
import iuic.underlying;

export namespace iuic {

struct object_registry_key {
  constexpr object_registry_key() noexcept = default;
  constexpr object_registry_key(units::hash val) noexcept : value{val} {};

  constexpr operator units::hash &() noexcept { return value; };
  constexpr operator const units::hash &() const noexcept { return value; };
  constexpr operator units::hash() const noexcept { return value; };
  constexpr auto
  operator<=>(const object_registry_key &) const noexcept = default;

  units::hash value;
};

struct text_registry_key {
  constexpr text_registry_key() noexcept = default;
  constexpr text_registry_key(units::hash val) noexcept : value{val} {};

  constexpr operator units::hash &() noexcept { return value; };
  constexpr operator const units::hash &() const noexcept { return value; };
  constexpr operator units::hash() const noexcept { return value; };
  constexpr auto
  operator<=>(const text_registry_key &) const noexcept = default;

  units::hash value;
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
