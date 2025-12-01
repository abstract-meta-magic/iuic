

module;

#include <map>
#include <string_view>
#include <variant>
#include <vector>

export module iuic.core:text.present;
import :base;
import :storage.def;
import :text.token;
import :text.font;

export namespace iuic::text {

struct present_node : token {
  ui_rect rect;
  //..
};

struct present {
  class aggregator;
  std::vector<present_node> nodes;
};

}; // namespace iuic::text

namespace iuic::text {

struct aggregation_bondle {
  token::sequence tokens{};
  glyph::sequence glyphs;
};

// frame-type
class present::aggregator {
  friend void iuic::advance(aggregator &);

public:
  // подготовка места для present, под token::sequence
  void reserve_present(size_t id, token::sequence sq);

  void apply_present(size_t id, glyph::sequence present);

  token::sequence get_tokens(size_t id) const;

  glyph::sequence get_present(size_t id) const;

private:
  std::map<size_t, aggregation_bondle> presents;
};
}; // namespace iuic::text
