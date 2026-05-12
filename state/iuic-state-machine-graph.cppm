// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:machine.graph;
import iuic.underlying;
import :decl;
import :value;

namespace iuic::state::machine {

export struct transition {
  value from;
  value to;
  bool is_bidirectional{false};
  constexpr bool operator==(const transition &other) const {
    return from == other.from && to == other.to;
  };
};

template <transition value> struct help_t {
  static constexpr auto get() {
    return []<transition... trs>(utils::ct::list<transition, trs...>) {
      if constexpr (sizeof...(trs) < 1) {
        return utils::ct::list<transition, value>{};
      } else if constexpr ((false || ... ||
                            (trs.from == value.from && trs.to == value.to))) {
        return utils::ct::list<transition, trs...>{};
      } else {
        return utils::ct::list<transition, trs..., value>{};
      }
    };
  };
};

template <value val> struct help_s {
  static constexpr auto get() {
    return []<value... trs>(utils::ct::list<value, trs...>) {
      if constexpr (sizeof...(trs) < 1) {
        return utils::ct::list<value, val>{};
      } else if constexpr ((false || ... || (trs == val))) {
        return utils::ct::list<value, trs...>{};
      } else {
        return utils::ct::list<value, trs..., val>{};
      }
    };
  };
};

template <transition... val> consteval inline auto make_stay_index_tree() {
  static constexpr auto expand =
      (utils::ct::list<value>{} & ... &
       (utils::ct::list<value, val.from>{} & utils::ct::list<value, val.to>{}));

  static constexpr auto remove_dublicate = expand.expand([]<value... list>() {
    return (utils::ct::list<value>{} & ... & (help_s<list>::get()));
  });

  return utils::ct::index_tree{remove_dublicate.expand(
      []<value... list>() { return std::array{list...}; })};
};

template <transition... value>
consteval inline auto make_transition_index_tree() {

  static constexpr auto expand =
      ((utils::ct::list<transition, value>{} &
        []<transition inner>(utils::ct::list<transition, inner>) {
          if constexpr (inner.is_bidirectional) {
            return utils::ct::list<transition, inner,
                                   transition{inner.to, inner.from}>{};
          } else {
            return utils::ct::list<transition, inner>{};
          }
        }) &
       ...);

  static constexpr auto remove_dublicate =
      []<transition... list>(utils::ct::list<transition, list...>) {
        return (utils::ct::list<transition>{} & ... & help_t<list>::get());
      }(expand);

  return utils::ct::index_tree{remove_dublicate.expand(
      []<transition... list>() { return std::array{list...}; })};
};

export template <transition... trs_> struct transition_graph {
  // TODO : нужна валидация графа
  static constexpr auto transition_index_tree =
      make_transition_index_tree<trs_...>();

  static constexpr auto stay_index_tree = make_stay_index_tree<trs_...>();

  static constexpr std::tuple value{trs_...};

  static constexpr std::size_t transition_count = transition_index_tree.size();

  static constexpr std::size_t stay_count = stay_index_tree.size();

  static constexpr std::size_t invalid_index{
      std::numeric_limits<std::size_t>::max()};

  static constexpr std::size_t transition_index(state::value from,
                                                state::value to) {
    return transition_index_tree.get_index({from, to});
  };

  static constexpr std::size_t stay_index(state::value stay) {
    return stay_index_tree.get_index(stay);
  };
};
} // namespace iuic::state::machine
