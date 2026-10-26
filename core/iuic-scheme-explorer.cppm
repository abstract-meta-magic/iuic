// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.explorer;
import std;
import iuic.underlying;
import iuic.asset;
import iuic.state;
import iuic.style;
import iuic.event;
import :policy;
import :scheme.base;
import :scheme.query;

namespace iuic::scheme {

struct explorer_assign {
  blueprint::base_iterator it;
  asset::catalog *assets;
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
      return std::get<ui::element::placed>(self().get_element(it).is);
    };

    template <typename T> T policy(iterators::base it) {
      return self().tenv->policy.get<T>(self().get_uid(it));
    };

    units::uid uid(iterators::base it) { return self().get_element(it).uid; };

    // TODO: mb wrap ??
    const text::data &text(iterators::base it) {
      return self().tenv->text.get(self().get_element(it).text);
    };

    auto clipzone() { return self().tenv->clip; };

  } props{*this};

  struct : utils::member_for<explorer> {
    bool is_virtualized(iterators::base it) {
      auto &el = self().get_element(it);

      return std::holds_alternative<ui::element::virtualized>(el.is);
    };

    bool is_discarded(iterators::base it) {
      auto &el = self().get_element(it);

      return std::holds_alternative<ui::element::discarded>(el.is);
    }

    bool has_text(iterators::base it) {
      return self().get_element(it).text != text::id::null;
    };

  } traits{*this};

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

    auto clipzones() { return self().tenv->clip.get(); };
  } ranges{*this};

  struct : utils::member_for<explorer> {
  private:
    bool eval(scheme::iterators::base it, query::valid_t t) {
      auto &el = self().get_element(it);
      return not(std::holds_alternative<ui::element::discarded>(el.is) ||
                 std::holds_alternative<ui::element::virtualized>(el.is));
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

      return in__(t.point, std::get<ui::element::placed>(el.is).bordered);
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

  struct : utils::member_for<explorer> {
    bool is_definition_exist(const asset::declaration &decl) const {
      return self().assets->contains(decl);
    };

    bool is_instance_exist(asset::instance_id iid) const {
      return self().assets->contains(iid);
    };

    bool is_instance_exist(asset::static_instance si) const {
      return self().assets->get_static_instance(si) != asset::invalid_id;
    };

    asset::definition &get_definition(const asset::declaration &decl) {
      return self().assets->get_definition(decl);
    };

    asset::instance &get_static_instance(const asset::static_instance &si) {
      return self().assets->get_instance(
          self().assets->get_static_instance(si));
    };

    asset::instance &get_instance(asset::instance_id iid) {
      return self().assets->get_instance(iid);
    };

    bool try_visit_definition(
        const asset::declaration &decl,
        std::invocable<const asset::definition &> auto &&call) noexcept {
      self().assets->try_visit_definition(decl,
                                          std::forward<decltype(call)>(call));
    };

    bool try_visit_static_instance(
        const asset::static_instance &si,
        func::as<void(const asset::instance &)> auto &&call) noexcept {
      self().assets->try_visit_instance(self().assets->get_static_instance(si),
                                        std::forward<decltype(call)>(call));
    };

    bool try_visit_instance(
        asset::instance_id iid,
        func::as<void(const asset::instance &)> auto &&call) noexcept {
      self().assets->try_visit_instance(iid,
                                        std::forward<decltype(call)>(call));
    };

    void query(decltype(asset::query::definition) type,
               func::as<void(const asset::definition &)> auto &&func,
               asset::query::is_pred auto &&...preds) const {
      self().assets->query(type, std::forward<decltype(func)>(func),
                           std::forward<decltype(preds)>(preds)...);
    };

    void query(decltype(asset::query::instance) type,
               func::as<void(const asset::instance &)> auto &&func,
               asset::query::is_pred auto &&...preds) const {
      self().assets->query(type, std::forward<decltype(func)>(func),
                           std::forward<decltype(preds)>(preds)...);
    };
  } asset{*this};

public: // assign
  explorer &operator=(explorer_assign assing) {
    begin_ = assing.it;
    assets = assing.assets;
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

  const ui::element &get_element(iterators::base it) {
    return *tree::const_access_iterator{tree::shift(begin_, it)};
  }

private:
  blueprint::base_iterator begin_{};
};
} // namespace iuic::scheme
