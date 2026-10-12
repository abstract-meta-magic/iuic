// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:base;
import iuic.underlying;
import std;

namespace iuic::asset {

struct callback_handl {};

export enum class instance_id : std::uint64_t {};

constexpr instance_id invalid_id{std::numeric_limits<std::uint64_t>::max()};

export struct tag {
  consteval tag() : decl{this} {}
  const tag *decl;
  constexpr auto operator<=>(const tag &) const = default;
};

export struct type {
  template <iuic::utils::unique_type t = {{}}> consteval type() : decl{this} {}
  const type *decl;
  constexpr auto operator<=>(const type &) const = default;
};

export inline constexpr type undefined{};

// WARNING
// GENERATED CODE WITH LLM
// INSPIRED BY : MurmurHash3 x64_128 mixing primitives
// LOCATION :
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
//
// REPLACE TO SELF IMPL

// TODO : replace to 128 bit hash
export using hash_t = std::uint64_t;

namespace hash {

constexpr hash_t rotl(hash_t x, int r) noexcept {
  return (x << r) | (x >> (64 - r));
}

constexpr hash_t fmix(hash_t h) noexcept {
  h ^= h >> 33;
  h *= 0xff51afd7ed558ccdULL;
  h ^= h >> 33;
  h *= 0xc4ceb9fe1a85ec53ULL;
  h ^= h >> 33;
  return h;
}

constexpr hash_t load64(const char *p) noexcept {
  hash_t v = 0;

  for (unsigned i = 0; i < 8; ++i)
    v |= static_cast<hash_t>(static_cast<unsigned char>(p[i])) << (i * 8);

  return v;
}

constexpr hash_t from(std::string_view str) noexcept {
  constexpr hash_t c1 = 0x87c37b91114253d5ULL;
  constexpr hash_t c2 = 0x4cf5ad432745937fULL;

  hash_t h = 0;
  const char *data = str.data();
  std::size_t len = str.size();

  while (len >= 8) {
    hash_t k = load64(data);

    k *= c1;
    k = rotl(k, 31);
    k *= c2;

    h ^= k;
    h = rotl(h, 27);
    h = h * 5 + 0x52dce729;

    data += 8;
    len -= 8;
  }

  hash_t tail = 0;

  for (std::size_t i = 0; i < len; ++i)
    tail |= static_cast<hash_t>(static_cast<unsigned char>(data[i])) << (i * 8);

  if (len != 0) {
    tail *= c1;
    tail = rotl(tail, 31);
    tail *= c2;
    h ^= tail;
  }

  h ^= str.size();

  return fmix(h);
}

} // namespace hash

}; // namespace iuic::asset

namespace std {
template <> struct hash<iuic::asset::tag> {
  std::size_t operator()(const iuic::asset::tag &t) const {
    return reinterpret_cast<std::uintptr_t>(t.decl);
  };
};

template <> struct hash<iuic::asset::type> {
  std::size_t operator()(const iuic::asset::type &t) const {
    return reinterpret_cast<std::uintptr_t>(t.decl);
  };
};

}; // namespace std
