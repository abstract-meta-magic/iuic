// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils;
import std;
import iuic.underlying;
import iuic.style;
import iuic.text;
import iuic.env;

namespace iuic {

export namespace layout {

struct element {
  units::ui::adaptive::size measure;
  units::ui::area arrange;
  std::span<const text::present::token> text;
  style::value style;

  struct meta {
    void set_discarted() { value.set(static_cast<int>(map::discarted), true); };

    void set_virtualized() {
      value.set(static_cast<int>(map::virtualized), true);
    };

    void set_text_mark() { value.set(static_cast<int>(map::text_mark), true); };

    void set_applyed() { value.set(static_cast<int>(map::applyed), true); };

    void set_measured() { value.set(static_cast<int>(map::measured), true); };

    void set_arranged() { value.set(static_cast<int>(map::arranged), true); };

    bool is_discarted() {
      return value.test(static_cast<int>(map::discarted));
    };

    bool is_virtualized() {
      return value.test(static_cast<int>(map::virtualized));
    };

    bool is_applyed() { return value.test(static_cast<int>(map::applyed)); };

    bool is_measured() { return value.test(static_cast<int>(map::measured)); };

    bool is_arranged() { return value.test(static_cast<int>(map::arranged)); };

    bool is_text() { return value.test(static_cast<int>(map::text_mark)); };

  private:
    enum class map {
      discarted = 0,
      text_mark,
      virtualized,
      measured,
      applyed,
      arranged,
      size__,
    };
    std::bitset<static_cast<std::size_t>(map::size__)> value{0};
  } meta;
};

using tree_type = ::iuic::tree::flat_bfs_type<element>;

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

namespace measure {

struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, element_base_iterator it)
      : utils_base{tenv, it} {};

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const units::ui::adaptive::size &operator[](base_iterator el) {
      return tree::access_iterator{iuic::tree::shift(self().it, el)}->measure;
    };
  } measure [[no_unique_address]];
};

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv,
             std::span<const iuic::text::raw::token> tokens_,
             element_base_iterator it)
      : utils_base{tenv, it}, tokens{tokens_} {};

  std::span<const iuic::text::raw::token> get_tokens() { return tokens; };

private:
  std::span<const iuic::text::raw::token> tokens;
};
}; // namespace measure

namespace arrange {
struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, element_base_iterator it)
      : utils_base{tenv, it} {};

  void apply_element(base_iterator el, units::ui::area a) {
    tree::access_iterator ait{tree::shift(it, el)};

    auto &veu = *ait;

    if (not ait->meta.is_applyed()) {
      struct : decltype(el) {
        std::size_t extract() { return self; };
      } ext{el};

      if (ext.extract() == 20) {
        void *heh = nullptr;
      };

      ait->arrange = a;
      ait->meta.set_applyed();
    } else {
      void *a = nullptr;
    };
  };

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const units::ui::adaptive::size &operator[](base_iterator el) {
      return tree::access_iterator{tree::shift(self().it, el)}->measure;
    };
  } measure [[no_unique_address]];

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->arrange;
  };
};

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv,
             std::span<const iuic::text::raw::token> tokens_,
             element_base_iterator it)
      : utils_base{tenv, it}, tokens{tokens_} {};

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->arrange;
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

  virtual std::optional<units::ui::adaptive::size>
  measure(measure::frame_utils utils) const = 0;

  virtual bool arrange(arrange::frame_utils utils) const = 0;
};

struct text {
  virtual ~text() {};
  virtual std::optional<units::ui::adaptive::size>
  measure(measure::text_utils utils) const = 0;

  virtual std::span<const iuic::text::present::token>
  arrange(arrange::text_utils utils) const = 0;
};
} // namespace layout
} // namespace iuic
