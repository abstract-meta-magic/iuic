// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.explorer;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;
import :policy;
// import :proto.base;
import :scheme.base;

namespace iuic::scheme {

namespace query {
namespace tag {

export struct element : iuic::query::tag {};

export template <const iuic::event::channel &channel__>
struct event : iuic::query::tag {
  static constexpr auto &channel = channel__;
};
} // namespace tag

export template <const iuic::event::channel &channel__>
constexpr iuic::query::expr<tag::event<channel__>> event{};

export constexpr iuic::query::expr<tag::element> element;

export struct hit {
  using type_tag = iuic::query::type;
  units::ui::position point;
};
export struct has_state {
  using type_tag = iuic::query::type;
  iuic::state::value state;
};

export struct valid_t {
  using type_tag = iuic::query::type;
} valid;

export template <erasure::is_pure_type T> struct type {
  using type_tag = iuic::query::type;
};

export struct state {
  using type_tag = iuic::query::type;
  iuic::state::value value;
};

export template <typename EXPR> struct qnot {
  using type_tag = iuic::query::type;
  EXPR expr;
};
}; // namespace query

export struct base {

protected:
  environment::tmp *tenv;
  environment::persist *penv;
};

struct explorer_assign {
  blueprint::base_iterator it;
  environment::tmp *tenv;
  environment::persist *penv;
};

export struct partition : base {
  // API
  partition(base &b, units::uid owner_) : base{b}, owner{owner_} {}

  struct : utils::member_for<partition> {
    bool has(state::value v) {
      return self().penv->state.has(self().owner, v);
    };
    void attach(state::value v) { self().penv->state.attach(self().owner, v); };
    void detach(state::value v) { self().penv->state.detach(self().owner, v); };
  } state{*this};

private:
  units::uid owner;
};

export struct capture : base {
  // API
  capture(base &b, units::uid owner__, units::uid obj__)
      : base{b}, owner_{owner__}, obj_{obj__} {}

  struct : utils::member_for<capture> {
    struct : utils::member_for<capture> {
      bool has(state::value v) {
        return self().penv->state.has(self().owner_, v);
      };
      void attach(state::value v) {
        self().penv->state.attach(self().owner_, v);
      };
      void detach(state::value v) {
        self().penv->state.detach(self().owner_, v);
      };
    } state{self()};

    struct : utils::member_for<capture> {

    } memory{self()};

    struct : utils::member_for<capture> {
      // emit ??
      // only archive(passive) ???
    } event{self()};
  } owner{*this};

  struct : utils::member_for<capture> {
    struct : utils::member_for<capture> {
      void try_visit(auto &&call) {
        self()
            .penv->object.get(self().obj_)
            .try_visit(std::forward<decltype(call)>(call));
      };
    } memory{self()};
  } obj{*this};

private:
  units::uid owner_;
  units::uid obj_;
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

    template <typename T> T policy(iterators::base it) {
      return self().tenv->policy.get<T>(self().get_uid(it));
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
    // TODO : REIMPL

    template <const iuic::event::channel &ch> void event(auto &&...args) {
      if constexpr (requires() {
                      self().tenv->event.query<ch>(
                          static_cast<base &>(self()),
                          std::forward<decltype(args)>(args)...);
                    }) {
        self().tenv->event.query<ch>(static_cast<base &>(self()),
                                     std::forward<decltype(args)>(args)...);
      } else {
        self().tenv->event.query<ch>(std::forward<decltype(args)>(args)...);
      }
    };

    void element(auto &&...args) {
      // do search element

    };

    bool eval(scheme::iterators::base it, query::valid_t t) {
      auto &el = self().get_element(it);
      return not(el.meta.has(el.meta.discarded) ||
                 el.meta.has(el.meta.virtualized));
    };

    bool eval(scheme::iterators::base it, query::hit t) {
      auto &el = self().get_element(it);

      constexpr auto in__ =
          [](const units::ui::position &p,
             const units::ui::rect &r) static constexpr noexcept -> bool {
        return p.x >= r.x && p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h;
      };

      /*
      bool pointInRect(float px, float py, float rx, float ry, float w, float h)
    { return px >= rx && px <= rx + w && py >= ry && py <= ry + h;
    }
      */

      return in__(t.point, el.area.bordered);
    };

    template <typename EXPR>
    bool eval(scheme::iterators::base it, query::qnot<EXPR> expr) {
      return not eval(it, expr.expr);
    };

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
};
} // namespace iuic::scheme
