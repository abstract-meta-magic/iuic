

module;

#include <cstdint>
#include <string_view>

export module iuic.key_map.base;
import iuic.core;

export namespace iuic::key_map {

consteval key_code mouse(std::string_view code) {
  if (code == "left") {
    return key_code{std::uint8_t{1}};
  } else if (code == "right") {
    return key_code{std::uint8_t{3}};
  } else if (code == "middle") {
    return key_code{std::uint8_t{2}};
  } else {
    return {};
  }
};

namespace en {
constexpr key_code qwerty(std::string_view code);
};

}; // namespace iuic::key_map
