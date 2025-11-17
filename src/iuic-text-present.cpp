
module;
#include <cstddef>
#include <utility>

module iuic.core;
import :text.present;

namespace iuic::text {

void present::aggregator::reserve_present(std::size_t id,
                                          token::sequence &&sq) {
  if (not is_reserved(id)) {
    presents.insert({id, aggregation_bondle{.sequence{std::move(sq)}}});
  } else {
    std::swap(presents.at(id).sequence, sq);
    presents.at(id).is_applyed = false;
  }
};

void present::aggregator::reserve_present(size_t id,
                                          const token::sequence &sq) {
  if (not is_reserved(id)) {
    presents.insert({id, aggregation_bondle{.sequence{std::move(sq)}}});
  } else {
    //  presents.at(id).sequence = sq;
    presents.at(id).is_applyed = false;
  }
};

void present::aggregator::apply_present(size_t id, present &&present) {
  if (is_reserved(id)) {
    std::swap(presents.at(id).present, present);
    presents.at(id).is_applyed = true;
  }
};

const token::sequence &present::aggregator::get_linked_text(size_t id) const {
  if (is_reserved(id)) {
    return presents.at(id).sequence;
  }

  return null.sequence;
};

const present &present::aggregator::get_present(size_t id) {
  if (is_applied(id)) {
    return presents.at(id).present;
  }

  return null.present;
};

bool present::aggregator::is_reserved(size_t id) const {
  return presents.contains(id);
};

bool present::aggregator::is_applied(size_t id) const {
  if (is_reserved(id)) {
    return presents.at(id).is_applyed;
  }
  return false;
};

}; // namespace iuic::text
