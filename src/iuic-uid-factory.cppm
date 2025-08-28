

module;

#include <cstdint>

export module iuic.core:uid.factory;
import :base;

namespace iuic::uid {

struct factory {
  uid_t make(uid_t) const noexcept;

  uid_t make(const char *, std::size_t) const noexcept;

  uid_t make() const noexcept;

  uid_t merge(uid_t, uid_t) const noexcept;

private:
  std::uint64_t seed;
};

}; // namespace iuic::uid
