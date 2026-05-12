// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.base;
import iuic.env;
import :decl;

namespace iuic::layout {
struct utils_base {
  using base_iterator = iuic::tree::base_iterator<iuic::tree::hierarchy::bfs>;
  using sibling_iterator =
      iuic::tree::sibling_iterator<iuic::tree::hierarchy::bfs>;
  using root_iterator = iuic::tree::root_iterator<iuic::tree::hierarchy::bfs>;
  using element_base_iterator = tree_type::base_iterator;
  using element_access_iterator = tree_type::access_iterator;

  utils_base(environment::tmp &tenv_, element_base_iterator it_)
      : tenv{tenv_}, it{it_} {}

  base_iterator self() { return it; };

  base_iterator parent() { return ++tree::root_iterator{it}; };

  tree::iterator_range_for<tree::iterator_range_trait<sibling_iterator>>
  childs() {
    return {iuic::tree::childs_of(base_iterator{it})};
  };

  // this is joke...BUT
  struct : utils::adv_member_for<utils_base> {
    style::value operator[](base_iterator el) {
      return element_access_iterator{tree::shift(self().it, el)}->style;
    };
  } style [[no_unique_address]];

  struct : utils::adv_member_for<utils_base> {
    template <typename... FMT_ARG>
    void error(std::format_string<FMT_ARG...> fs, FMT_ARG &&...args) {
      // TODO : body
      std::println("\x1B[31m[IUIC-LAYOUT-ERROR]>>>");
      std::println(std::move(fs), std::forward<FMT_ARG>(args)...);
      std::println("\033[0m\n");
    };

    template <typename... FMT_ARG>
    void warn(std::format_string<FMT_ARG...> fs, FMT_ARG &&...args) {
      // TODO : body
      std::println("\x1B[33m[IUIC-LAYOUT-WARNING]>>>");
      std::println(std::move(fs), std::forward<FMT_ARG>(args)...);
      std::println("\033[0m\n");
    };

    template <typename... FMT_ARG>
    void info(std::format_string<FMT_ARG...> fs, FMT_ARG &&...args) {
      // TODO : body
      std::println("\x1B[34m[IUIC-LAYOUT-INFO]>>>");
      std::println(std::move(fs), std::forward<FMT_ARG>(args)...);
      std::println("\033[0m\n");
    };
  } log [[no_unique_address]];

  units::ui::size root_size() { return tenv.meta.viewport_size; };

  units::upixel segment_size() { return tenv.meta.segment_size; };

protected:
  environment::tmp &tenv;
  element_base_iterator it;
};

}; // namespace iuic::layout
