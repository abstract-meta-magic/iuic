// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.explorer;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;
import :policy;
import :proto.base;
import :scheme.base;

namespace iuic::scheme {

struct explorer_assign {
  blueprint::base_iterator it;
  environment::tmp *tenv;
  environment::persist *penv;
};

export struct explorer {
  struct : utils::member_for<explorer> {
    style::value style(iterators::base it) {
      return self().tenv->style.get(self().get_sid(it));
    };

    const units::ui::area &area(iterators::base it) {
      return tree::const_access_iterator{tree::shift(self().begin_, it)}->area;
    };

    bool is_virtualized(iterators::base it) {
      auto &el = *tree::const_access_iterator{tree::shift(self().begin_, it)};

      return el.meta.has(el.meta.virtualized);
    };

    bool is_discarded(iterators::base it) {
      auto &el = *tree::const_access_iterator{tree::shift(self().begin_, it)};

      return el.meta.has(el.meta.discarded);
    }

    policy::hovered hovered_policy(iterators::base it) {
      // TODO : fixme
      return {};
    };

    bool has_text(iterators::base it) {
      return not self().get_element(it).text.empty();
    };

    std::span<const text::present::token> text(iterators::base it) {
      return self().get_element(it).text;
    };

  } props{*this};

  struct : utils::member_for<explorer> {
    // TODO :
  } event{*this};

  struct : utils::member_for<explorer> {
    bool has(iterators::base it, state::value state) {
      return self().penv->state.has(self().get_uid(it), state);
    };

    void attach(iterators::base it, state::value state) {
      self().penv->state.attach(self().get_uid(it), state);
    }

    void detach(iterators::base it, state::value state) {
      self().penv->state.detach(self().get_uid(it), state);
    }
  } state{*this};

  struct : utils::member_for<explorer> {
    auto level_order() {

      tree::hierarchy::bfs::base_iterator bhit{self().begin_};

      return tree::iterator_range_for{
          bhit, tree::iterator_type<tree::base_iterator>{}};
    };

    auto reverse_level_order() {
      return ranges::reverse_level_order{
          tree::hierarchy::bfs::base_iterator{self().begin_}};
    };

    auto postorder() {
      return ranges::postorder{
          tree::hierarchy::bfs::base_iterator{self().begin_}};
    };

    auto preorder() {};

    // only available elements
    // no virtualized,discarted
    auto viwe_ordered() {};
  } ranges{*this};

  struct : utils::member_for<explorer> {
    void event() {
      // DO
    };

    template <typename... Ts>
    std::vector<scheme::iterators::base> element(query::expr_t<Ts...> &&expr) {
      // TODO : make parallel ???
      std::vector<scheme::iterators::base> res;
      // unexpected expression
      auto &penv = *self().penv;
      auto bit = self().begin_;
      for (auto it : self().ranges.level_order()) {
        if (expect(expr, tree::shift(bit, it), penv)) {
          res.push_back(it);
        };
      }
      return res;
    };

    template <typename... ARGS>
    static constexpr bool expect(const auto &pred, blueprint::base_iterator it,
                                 environment::persist &penv) {

      static constexpr auto in__ =
          [](const units::ui::rect &area,

             const units::ui::position point__) static constexpr {
            return point__.x >= area.x && point__.x <= area.x + area.w &&

                   point__.y >= area.y && point__.y <= area.y + area.h;
          };

      static constexpr auto not__ =
          []<typename T>(const query::qnot<T> &qnot,
                         blueprint::base_iterator it,
                         environment::persist &penv) constexpr static {
            return not expect(qnot.expr, it, penv);
          };

      using type = std::remove_cvref_t<decltype(pred)>;
      tree::access_iterator ait{it};
      if constexpr (std::same_as<type, query::valid_t>) {
        return not ait->meta.has(ait->meta.discarded);
      } else if constexpr (std::same_as<type, query::hit>) {
        return in__(tree::access_iterator{it}->area.bordered, pred.point);
      } else if constexpr (std::same_as<type, query::has_state>) {
        return penv.state.has(ait->uid, pred.state);
      } else if constexpr (requires { not__(pred, it, penv); }) {
        return not expect(pred.expr, it, penv);
      } else {
        return false;
      };
    };

    template <typename... Ts>
    static constexpr bool expect(const query::expr_t<Ts...> &expr,
                                 blueprint::base_iterator it,
                                 environment::persist &penv) {
      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return (expect(expr.template at<I>(), it, penv) && ...);
      }(std::make_index_sequence<expr.size()>{});
    };
  } queries{*this};

  struct : utils::member_for<explorer> {
    void set_key_code(units::keycode code) {
      self().penv->external.key_code = code;
    };
  } global{*this};

public: // assign
  explorer &operator=(explorer_assign assing) {
    begin_ = assing.it;
    tenv = assing.tenv;
    penv = assing.penv;

    return *this;
  }

private:
  units::uid get_uid(iterators::base it) {
    return tree::access_iterator{tree::shift(begin_, it)}->uid;
  };

  style::sid get_sid(iterators::base it) {
    return tree::access_iterator{tree::shift(begin_, it)}->sid;
  };

  const bp_element &get_element(iterators::base it) {
    return *tree::const_access_iterator{tree::shift(begin_, it)};
  }

private:
  blueprint::base_iterator begin_{};
  environment::tmp *tenv{nullptr};
  environment::persist *penv{nullptr};
};
} // namespace iuic::scheme
