
module;
#include <cstddef>
#include <cstdint>
#include <string>

#include "tmp_solutions/hash/xxh3.h"

module iuic.core;
import :uid.factory;

namespace iuic::uid {

static constexpr XXH64_hash_t static_seed{443573};

uid_t make(uid_t uu) noexcept {
  return XXH64(reinterpret_cast<const char *>(&uu), sizeof(uu), static_seed);
};

uid_t make(const char *c_ptr, std::size_t size) noexcept {
  return XXH64(c_ptr, size, static_seed);
};

uid_t make(uid_t seed, const char *c_ptr, std::size_t size) noexcept {
  return XXH64(c_ptr, size, seed);
};

uid_t merge(uid_t seed, uid_t val) noexcept {

  return XXH64(reinterpret_cast<const char *>(&val), sizeof(val), seed);
};

}; // namespace iuic::uid
