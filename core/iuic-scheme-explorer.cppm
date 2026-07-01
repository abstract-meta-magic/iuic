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
  private:
    static constexpr bool eval(const proto::base::query::hit &hit,
                               blueprint::access_iterator it,
                               environment::persist &penv) {
      auto in__ = [](const units::ui::rect area,
                     const units::ui::position pointer) static constexpr {
        return pointer.x >= area.x && pointer.x <= area.x + area.w &&
               pointer.y >= area.y && pointer.y <= area.y + area.h;
      };

      return in__(tree::access_iterator{it}->area.bordered, hit.point);
    };

    static constexpr bool eval(proto::base::query::valid_t _,
                               blueprint::access_iterator it,
                               environment::persist &penv) {
      return not it->meta.has(it->meta.discarded);
    };
    static constexpr bool eval(const proto::base::query::has_state &st,
                               blueprint::access_iterator it,
                               environment::persist &penv) {
      return penv.state.has(it->uid, st.state);
    };

    template <typename T>
    static constexpr bool eval(const iuic::query::qnot<T> &expr,
                               blueprint::access_iterator it,
                               environment::persist &penv) {
      return not eval(expr.expr, it, penv);
    };

  public:
    template <const event::channel &EC, typename... Ts, typename... ARGS>
    decltype(auto) operator()(
        iuic::query::expr<proto::base::query::tag::event<EC>, Ts...> expr,
        ARGS &&...args) {
      if constexpr (EC.type == event::channel::type_e::active) {
        // TODO : do job
        self().tenv->event.query<EC>([](auto &q) {});
      } else {
        // TODO : do job
      }
    };

    template <typename... Ts>
    decltype(auto) operator()(
        iuic::query::expr<proto::base::query::tag::element, Ts...> expr) {
      // do element job

      std::vector<iterators::base> res;

      auto bit = self().begin_;
      auto &penv = *self().penv;

      for (auto it : self().ranges.level_order()) {
        auto ait = tree::access_iterator{tree::shift(bit, it)};

        if ([&]<std::size_t... I>(std::index_sequence<I...>) {
              return (eval(expr.template at<I>(), ait, penv) && ...);
            }(expr.index_sequence())) {
          res.push_back(it);
        }
      };

      return res;
    };

    template <typename... Ts>
    decltype(auto) operator()(
        iuic::query::expr<proto::base::query::tag::event_local, Ts...> expr,
        units::keycode key) {
      // do element job
      self().tenv->event.query<proto::base::event::local>([&](auto &q) {
        q.meta([&](proto::base::event::pkg_meta &meta) {
           return self().penv->state.has(meta.owner, proto::base::state::local);
         })
            .template type<proto::base::event::key>()
            .trigger(key, self().penv, self().tenv);
        // add expr eval
      });
    };

    template <typename... Ts>
    decltype(auto) operator()(
        iuic::query::expr<proto::base::query::tag::event_local, Ts...> expr,
        units::ui::position pointer) {
      // do element job
      self().tenv->event.query<proto::base::event::local>([&](auto &q) {
        q.meta([&](proto::base::event::pkg_meta &meta) {
           return self().penv->state.has(meta.owner, proto::base::state::local);
         })
            .template type<proto::base::event::pointer>()
            .trigger(pointer, self().penv, self().tenv);
        // add expr eval
      });
    };

    template <typename... Ts>
    decltype(auto) operator()(
        iuic::query::expr<proto::base::query::tag::event_global, Ts...> expr,
        units::keycode key) {
      self().tenv->event.query<proto::base::event::global>([&](auto &q) {
        q.template type<proto::base::event::key>().trigger(key, self().penv,
                                                           self().tenv);
        // add expr eval
      });
    };

    template <typename... Ts>
    decltype(auto) operator()(
        iuic::query::expr<proto::base::query::tag::event_global, Ts...> expr,
        units::ui::position pointer) {
      self().tenv->event.query<proto::base::event::global>([&](auto &q) {
        q.template type<proto::base::event::pointer>().trigger(
            pointer, self().penv, self().tenv);
        // add expr eval
      });
    };
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
  environment::tmp *tenv{nullptr};
  environment::persist *penv{nullptr};
};
} // namespace iuic::scheme
