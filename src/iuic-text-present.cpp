module iuic.core;
import std;
import :text.present;

namespace iuic::text {

void present::aggregator::reserve_present(std::size_t id, token::sequence sq) {
  if (presents.contains(id)) {
    std::swap(presents.at(id).tokens, sq);
  } else {
    presents.insert({id, aggregation_bondle{.tokens{std::move(sq)}}});
  }
};

void present::aggregator::apply_present(std::size_t id,
                                        glyph::sequence present) {
  if (presents.contains(id)) {
    presents.at(id).glyphs = present;
  }
};

token::sequence present::aggregator::get_tokens(std::size_t id) const {
  if (presents.contains(id)) {
    return presents.at(id).tokens;
  }

  return {};
};

glyph::sequence present::aggregator::get_present(std::size_t id) const {
  if (presents.contains(id)) {
    return presents.at(id).glyphs;
  }

  return {};
};
}; // namespace iuic::text
