// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.explorer;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;
import :policy;
import :scheme.base;
import :scheme.query;

namespace iuic::scheme {

struct explorer_assign {
  blueprint::base_iterator it;
  environment::tmp *tenv;
  environment::persist *penv;
};

export struct explorer : base {
  struct : utils::member_for<explorer> {
    style::value style(iterators::base it) {
      return self().tenv->style.get(self().get_sid(it));
    };

    units::ui::order order(iterators::base it) {
      return self().get_element(it).order;
    };

    const units::ui::area &area(iterators::base it) {
      return self().get_element(it).area;
    };

    bool is_virtualized(iterators::base it) {
      auto &el = self().get_element(it);

      return el.meta.has(el.meta.virtualized);
    };

    bool is_discarded(iterators::base it) {
      auto &el = self().get_element(it);

      return el.meta.has(el.meta.discarded);
    }

    template <typename T> T policy(iterators::base it) {
      return self().tenv->policy.get<T>(self().get_uid(it));
    };

    bool has_text(iterators::base it) {
      return not self().get_element(it).text.empty();
    };

    units::uid uid(iterators::base it) { return self().get_element(it).uid; };

    std::span<const text::present::token> text(iterators::base it) {
      return self().get_element(it).text;
    };

    auto get_clipzones() { return self().tenv->clip.get(); };
  } props{*this};

  struct : utils::member_for<explorer> {
    bool has(iterators::base it, state::value state) {
      return self().penv->state.access(self().get_uid(it)).has(state);
    };

    void attach(iterators::base it, state::value state) {
      self().penv->state.access(self().get_uid(it)).attach(state);
    }

    void detach(iterators::base it, state::value state) {
      self().penv->state.access(self().get_uid(it)).detach(state);
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
  } ranges{*this};

  struct : utils::member_for<explorer> {
  private:
    bool eval(scheme::iterators::base it, query::valid_t t) {
      auto &el = self().get_element(it);
      return not(el.meta.has(el.meta.discarded) ||
                 el.meta.has(el.meta.virtualized));
    };

    bool eval(scheme::iterators::base it, query::state t) {
      return self().state.has(it, t.value);
    };

    bool eval(scheme::iterators::base it, query::hit t) {
      auto &el = self().get_element(it);

      constexpr auto in__ =
          [](const units::ui::position &p,
             const units::ui::rect &r) static constexpr noexcept -> bool {
        return p.x >= r.x && p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h;
      };

      return in__(t.point, el.area.bordered);
    };

    template <typename EXPR>
    bool eval(scheme::iterators::base it, query::qnot<EXPR> expr) {
      return not eval(it, expr.expr);
    };

  public:
    template <const iuic::event::channel &ch, typename... EXPR>
    decltype(auto)
    operator()(const iuic::query::expr<query::tag::event<ch>, EXPR...> &expr) {

      return self().tenv->event.query<ch>([&](iuic::event::query<ch> &q) {
        if constexpr (requires() { q(static_cast<base &>(self()), expr); }) {
          return q(static_cast<base &>(self()), expr);
        } else {
          return q(expr);
        }
      });
    }

    template <typename... EXPR>
    std::vector<scheme::iterators::base>
    operator()(const iuic::query::expr<query::tag::element, EXPR...> &expr) {

      std::vector<scheme::iterators::base> res;
      for (auto el : self().ranges.level_order()) {
        if (expr.unroll([&](auto &&...args) {
              return (true && ... && eval(el, args));
            })) {
          res.push_back(el);
        };
      }
      return res;
    }
  } query{*this};

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
};
} // namespace iuic::scheme
