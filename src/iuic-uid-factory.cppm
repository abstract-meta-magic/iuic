

module;

#include <cstdint>

export module iuic.core:uid.factory;
import :base;

namespace iuic::uid {

uid_t make(uid_t) noexcept;

uid_t make(uid_t seed, const char *c_ptr, std::size_t size) noexcept;

uid_t make(const char *, std::size_t) noexcept;

uid_t merge(uid_t, uid_t) noexcept;

}; // namespace iuic::uid
