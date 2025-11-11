

module;

#include <map>
#include <string_view>
#include <variant>
#include <vector>

export module iuic.core:text.present;
import :base;
import :storage.def;
import :text.token;

export namespace iuic::text {

struct present_node : token {
  ui_rect rect;
  //..
};

struct present {
  std::vector<present_node> nodes;
};

}; // namespace iuic::text

namespace iuic::text {

struct aggregation_bondle {
  token::sequence sequence{};
  present present{};
  bool is_applyed{false};
};

class present_aggregator {
  friend void iuic::advance(present_aggregator &);

public:
  // подготовка места для present, под token::sequence
  void reserve_present(size_t id, token::sequence &&sq);

  void reserve_present(size_t id, const token::sequence &sq);

  void apply_present(size_t id, present &&present);

  const token::sequence &get_linked_text(size_t id) const;

  const present &get_present(size_t id);

  bool is_reserved(size_t id) const;

  bool is_applied(size_t id) const;

private:
  static constexpr aggregation_bondle null{};

  std::map<size_t, aggregation_bondle> presents;
};
}; // namespace iuic::text
