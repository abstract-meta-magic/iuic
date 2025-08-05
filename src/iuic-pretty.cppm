
module;

#include <cstddef>

export module iuic.pretty;
import iuic.core;

namespace iuic {
// ake : jsx --> cxxx or c3x

constexpr int operator""_layout(const char *, std::size_t len) { return 1; };
constexpr style operator""_style(const char *, std::size_t len) { return {}; };

void test__() {
  auto raw_layout = R"(
<App>
  <button>click on me</button>
  <div>
  </div>
</App>)"_layout;

  auto style = R"(
style {
  max-with = 220px;
  min-with = 100px;
}
)"_style;

  // to write style
  // _ss
  // летиралы для макета и стиля
  // должны быть в расширениях
  // iuic.pretty - YES
  // в базовой версии их нету
  // iuic.core - NO

  // bind_raw_layout(layout_register,raw_layout);
};
}; // namespace iuic
