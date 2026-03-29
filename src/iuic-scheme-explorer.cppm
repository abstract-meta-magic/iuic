// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.explorer;
import std;
import iuic.underlying;
import :policy;
import :style;
import :text.present;
import :event;
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
      return utils::tree::const_access_iterator{
          utils::tree::shift(self().begin_, it)}
          ->area;
    };
  } props{*this};

  struct : utils::member_for<explorer> {
    std::span<event::value> list(iterators::base it) {
      return self().tenv->event.list_of(self().get_uid(it));
    };

    void trigger(event::value &e) { event::trigger(e, *(self().penv)); }
  } event{*this};

  struct : utils::member_for<explorer> {
    bool has(iterators::base it, state::value state) {
      return self().penv->state.has(self().get_uid(it), state);
    };

    void attach(iterators::base it, state::value state) {
      self().penv->state.attach(self().get_uid(it), state);
    }

    void detach(iterators::base it, state::value state) {
      self().penv->state.attach(self().get_uid(it), state);
    }
  } state{*this};

  struct : utils::member_for<explorer> {
    auto level_order() {
      return utils::tree::bfs_iterator_range_for{
          self().h, utils::tree::iterator_type<utils::tree::base_iterator>{}};
    };
  } ranges{*this};

  struct : utils::member_for<explorer> {
    void set_key_code(key_code code) { self().penv->external.key_code = code; };
  } global{*this};

public: // assign
  explorer &operator=(explorer_assign assing) {
    begin_ = assing.it;
    tenv = assing.tenv;
    penv = assing.penv;

    // copy hierarchy
    h = {begin_};

    return *this;
  }

private:
  units::uid get_uid(iterators::base it) {
    return utils::tree::access_iterator{utils::tree::shift(begin_, it)}->uid;
  };

  style::sid get_sid(iterators::base it) {
    return utils::tree::access_iterator{utils::tree::shift(begin_, it)}->sid;
  };

private:
  blueprint::base_iterator begin_{};
  utils::tree::hierarchy<blueprint> h{};
  environment::tmp *tenv{nullptr};
  environment::persist *penv{nullptr};
};
} // namespace iuic::scheme
