
module;
#include <cstddef>
#include <cstdint>
#include <string>

#include "tmp_solutions/hash/xxh3.h"

module iuic.core;
import :hash;

namespace iuic::hash {

static constexpr XXH64_hash_t static_seed{443573};

hash_t make(const char *c_ptr, std::size_t size) noexcept {
  return XXH64(c_ptr, size, static_seed);
};

hash_t make(hash_t seed, const char *c_ptr, std::size_t size) noexcept {
  return XXH64(c_ptr, size, seed);
};

hash_t merge(hash_t seed, hash_t val) noexcept {

  return XXH64(reinterpret_cast<const char *>(&val), sizeof(val), seed);
};

}; // namespace iuic::hash
