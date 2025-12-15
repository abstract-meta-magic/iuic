

module;

export module iuic.core:hash;
import std;
import :base;

namespace iuic::hash {

using hash_t = std::uint64_t;

hash_t make(hash_t seed, const char *c_ptr, std::size_t size) noexcept;

hash_t make(const char *, std::size_t) noexcept;

hash_t merge(hash_t, hash_t) noexcept;

}; // namespace iuic::hash
