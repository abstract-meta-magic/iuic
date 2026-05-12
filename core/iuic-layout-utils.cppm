// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:layout.utils;
import std;
import iuic.underlying;
import iuic.text;
import :environment.tmp;
import :scheme.base;

namespace iuic {

export namespace layout {
struct utils_base {
  using iterator = scheme::iterators::base;
  using root = scheme::iterators::root;
  using sibling = scheme::iterators::sibling;

  utils_base(environment::tmp &tenv_, scheme::blueprint::base_iterator it_)
      : tenv{tenv_}, it{it_} {}

  iterator self() { return it; };

  iterator parent() { return ++tree::root_iterator{it}; };

  tree::iterator_range_for<tree::iterator_range_trait<sibling>> childs() {
    return {scheme::iterators::childs_of(it)};
  };

  // this is joke...BUT
  struct : utils::adv_member_for<utils_base> {
    style::value operator[](iterator el) {
      return self().tenv.style.get(
          tree::access_iterator{tree::shift(self().it, el)}->sid);
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
  scheme::blueprint::base_iterator it;
};

namespace measure {

using result = units::ui::adaptive::size;

struct unit {
  style::sid sid{0};
  result measure{units::ui::none{}, units::ui::none{}};
  // приоритеты и требования ???
};

using tree = tree::flat_bfs_type<unit>;

struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, scheme::blueprint::base_iterator it,
              measure::tree::base_iterator m_)
      : utils_base{tenv, it}, m{m_} {};

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const measure::result &operator[](iterator el) {
      return tree::access_iterator{iuic::tree::shift(self().m, el)}->measure;
    };
  } measure [[no_unique_address]];

private:
  measure::tree::base_iterator m;
};

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv,
             std::span<const iuic::text::raw::token> tokens_,
             scheme::blueprint::base_iterator it)
      : utils_base{tenv, it}, tokens{tokens_} {};

  std::span<const iuic::text::raw::token> get_tokens() { return tokens; };

private:
  std::span<const iuic::text::raw::token> tokens;
};
}; // namespace measure

namespace arrange {
struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, scheme::blueprint::base_iterator it,
              measure::tree::sibling_iterator m_)
      : utils_base{tenv, it}, m{m_} {};

  void apply_element(iterator el, units::ui::area a) {
    tree::access_iterator ait{tree::shift(it, el)};

    if (ait) {
      ait->area = a;
      ait->meta.set(ait->meta.applied);
    }
  };

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const measure::result &operator[](iterator el) {
      return tree::access_iterator{tree::shift(self().m, el)}->measure;
    };
  } measure [[no_unique_address]];

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->area;
  };

private:
  measure::tree::sibling_iterator m;
};

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv,
             std::span<const iuic::text::raw::token> tokens_,
             scheme::blueprint::base_iterator it)
      : utils_base{tenv, it}, tokens{tokens_} {};

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->area;
  };

  std::span<const iuic::text::raw::token> get_tokens() { return tokens; };

  std::span<const iuic::text::present::token>
  commit(const std::vector<iuic::text::present::token> &tks) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>(tks.size());

    if (mem.size() == tks.size()) {
      for (std::size_t i{0}, e{tks.size()}; i < e; ++i) {
        new (&mem[i]) iuic::text::present::token{tks[i]};
      };
    }

    return mem;
  };

  std::span<const iuic::text::present::token>
  commit(std::vector<iuic::text::present::token> &&tks) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>(tks.size());

    if (mem.size() == tks.size()) {
      for (std::size_t i{0}, e{tks.size()}; i < e; ++i) {
        new (&mem[i]) iuic::text::present::token{std::move(tks[i])};
      };
    }

    return mem;
  };

  std::span<const iuic::text::present::placement>
  commit(std::vector<iuic::text::present::placement> &&pls) {
    auto mem = tenv.memory.allocate<iuic::text::present::placement>(pls.size());

    if (mem.size() == pls.size()) {
      for (std::size_t i{0}, e{pls.size()}; i < e; ++i) {
        new (&mem[i]) iuic::text::present::placement{std::move(pls[i])};
      };
    }

    return mem;
  };

  std::span<const iuic::text::present::token>
  commit(iuic::text::present::token &&tk) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>();
    if (not mem.empty()) {
      new (&mem[0]) iuic::text::present::token{std::move(tk)};
    }
    return mem;
  };

  std::span<const iuic::text::present::token>
  commit(const iuic::text::present::token &tk) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>();
    if (not mem.empty()) {
      new (&mem[0]) iuic::text::present::token{tk};
    }
    return mem;
  };

private:
  std::span<const iuic::text::raw::token> tokens;
};
}; // namespace arrange

struct frame {
  constexpr virtual ~frame() {};

  constexpr frame() noexcept {};

  virtual std::optional<measure::result>
  measure(measure::frame_utils utils) const = 0;

  virtual bool arrange(arrange::frame_utils utils) const = 0;
};

struct text {
  virtual ~text() {};
  virtual std::optional<measure::result>
  measure(measure::text_utils utils) const = 0;

  virtual std::span<const iuic::text::present::token>
  arrange(arrange::text_utils utils) const = 0;
};
} // namespace layout
} // namespace iuic
