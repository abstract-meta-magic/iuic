
module;
#include <cstddef>

#include "tmp_solutions/hash/xxh3.h"

module iuic.core;
import :uid.factory;

namespace iuic {

uid_t uid::factory::make(uid_t) const noexcept {
  XXH64_state_t *const st = XXH64_createState();

  return {};
};

uid_t uid::factory::make(const char *, std::size_t) const noexcept {

  return {};
};

uid_t uid::factory::make() const noexcept { return {}; };

uid_t uid::factory::merge(uid_t, uid_t) const noexcept { return {}; };
}; // namespace iuic
