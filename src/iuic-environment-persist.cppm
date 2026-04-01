// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:environment.persist;
import std;
import iuic.underlying;
import iuic.state;
import :style;
import :machine.dispatcher;
import :key_code;

namespace iuic::environment {

struct object {
  void *data;
  const erasure::type *type;
  enum state_e {
    non_exist,
    deleted,
    outdated_this_type,
    outdated_other_type,
    alive_this_type,
    alive_other_type,
    reserve_this_type,
    reserve_other_type,
    reserve_undefined_type,
  };
};

struct persist {
  struct : private iuic::advance::interface {
    friend persist;
    using buff_t =
        utils::swap_buffers<std::unordered_map<units::uid, object>, 3>;

    using enum object::state_e;

    erasure::visited::as_mutable get(units::uid uid) {
      auto [old, cur] = objects.get_buffers();
      if (auto it = cur.find(uid); it != cur.end()) {
        auto &obj = it->second;
        return erasure::visited::as_mutable{
            erasure::visited{obj.data, obj.type}};
      } else if (auto it = old.find(uid); it != old.end()) {
        auto &obj = it->second;
        return erasure::visited::as_mutable{
            erasure::visited{obj.data, obj.type}};
      }
      return {nullptr};
    };

    void reserve(units::uid uid, const erasure::type *type) {
      auto [old, cur] = objects.get_buffers();

      if (not cur.contains(uid)) {
        cur.insert({uid, {nullptr, type}});
      }
    };

    template <typename T> void reserve(units::uid uid) {
      reserve(uid, erasure::type::from<T>());
    };

    void construct(units::uid uid, auto &&call) {
      auto [old, cur] = objects.get_buffers();
      if (cur.contains(uid) && cur.at(uid).data == nullptr) {
        using ftaits =
            typename decltype(iuic::erasure::func_type{call})::traits;

        if constexpr (not std::same_as<typename ftaits::return_t, void>) {
          using type = std::remove_cvref_t<typename ftaits::return_t>;

          auto *otype = erasure::type::from<type>();

          auto &obj = cur.at(uid);

          if (obj.type == otype) {
            obj.data = new type{call()};
          }
        }
      }
    };

    void destruct(units::uid uid) {
      auto [old, cur] = objects.get_buffers();

      if (cur.contains(uid)) {
        auto ex = cur.extract(uid);
        auto &obj = ex.mapped();
        obj.type->dctor(obj.data);
      } else if (old.contains(uid)) {
        auto ex = old.extract(uid);
        auto &obj = ex.mapped();
        obj.type->dctor(obj.data);
      }
    };

    void update_lifetime(units::uid uid) { objects.move_forward(uid); };

    object::state_e state(units::uid uid, const erasure::type *otype) {
      auto [old, cur] = objects.get_buffers();

      if (cur.contains(uid)) {
        if (auto &obj = cur.at(uid); obj.type == otype) {
          return obj.data ? object::alive_this_type : object::reserve_this_type;
        } else {
          return obj.data ? object::alive_other_type
                          : object::reserve_other_type;
        }
      } else if (old.contains(uid)) {
        if (old.at(uid).type == otype) {
          return object::outdated_this_type;
        } else {
          return object::outdated_other_type;
        }
      }

      return object::non_exist;
    };

    template <erasure::is_pure_type T> object::state_e state(units::uid uid) {
      return state(uid, erasure::type::from<T>());
    }

  private:
    void advance() override { objects.swap(); };

  private:
    buff_t objects;
  } object;

  struct : private iuic::advance::interface {
    using buff_t = utils::swap_buffers<
        std::unordered_map<units::uid, std::unordered_set<state::value>>, 3>;
    friend persist;
    void attach(units::uid uid, iuic::state::value value) {
      auto [old, cur] = data.get_buffers();

      data.move_forward(uid); // auto check old value

      if (not cur.contains(uid)) {
        cur.insert({uid, {}});
      }

      cur.at(uid).insert(value);
    };

    void detach(units::uid uid, iuic::state::value value) {
      auto [old, cur] = data.get_buffers();
      if (auto it = cur.find(uid); it != cur.end()) {
        it->second.erase(value);
      }
    };

    bool has(units::uid uid, iuic::state::value value) {
      auto [old, cur] = data.get_buffers();
      if (cur.contains(uid)) {
        return cur.at(uid).contains(value);
      } else if (old.contains(uid)) {
        return old.at(uid).contains(value);
      } else {
        return false;
      };
    };

    void update_lifetime(units::uid uid) {
      auto [old, cur] = data.get_buffers();
      data.move_forward(uid);
    };

  protected:
    void advance() override { data.swap(); };

  private:
    buff_t data;
  } state;

  struct : private iuic::advance::interface {
    using buff_t = utils::swap_buffers<
        std::unordered_map<units::uid,
                           std::unique_ptr<state::machine::instance>>,
        3>;
    friend persist;
    state::machine::instance *get(units::uid uid) {
      auto [old, cur] = data.get_buffers();

      if (cur.contains(uid)) {
        return cur.at(uid).get();
      } else if (old.contains(uid)) {
        return old.at(uid).get(); // outdated, but OK
      } else {
        return nullptr;
      };
    };

    void construct(units::uid uid,
                   const state::machine::is_prototype auto &proto) {
      using spec = std::remove_cvref_t<decltype(proto)>::spec;
      auto [old, cur] = data.get_buffers();

      if (old.contains(uid) && old.at(uid) &&
          old.at(uid)->get_spec_id() == spec::runtime_id()) {
        data.move_forward(uid);
      } else if (not cur.contains(uid)) {
        cur.insert({uid, spec::make_instance(proto)});
      } else if (auto &instance = cur.at(uid);
                 instance && instance->get_spec_id() != spec::runtime_id()) {
        instance.swap(spec::make_instance(proto));
      };
    };

    void destruct(units::uid uid) {
      auto [old, cur] = data.get_buffers();

      if (cur.contains(uid)) {
        cur.extract(uid);
      } else if (old.contains(uid)) {
        old.extract(uid);
      }
    };

    void update_lifetime(units::uid uid) { data.move_forward(uid); };

  protected:
    void advance() override { data.swap(); };

  private:
    buff_t data;
  } machine;

  struct : private iuic::advance::interface {
    friend persist;
    units::ui::size get_viewport_size() { return viewport_size; };

    void set_viewport_size(units::ui::size vp) { viewport_size = vp; };

    units::ui::position get_pointer_position() { return pointer_position; };

    units::ui::position get_old_pointer_position() {
      return old_pointer_position;
    };

    void set_pointer_position(units::ui::position ppos) {
      old_pointer_position = std::exchange(pointer_position, ppos);
    };

    key_code key_code;

  private:
    units::ui::position old_pointer_position;
    units::ui::position pointer_position;
    units::ui::size viewport_size;
  } external;

  persist(advance::pool &pool) {
    object.rebind(pool);
    state.rebind(pool);
    machine.rebind(pool);
    external.rebind(pool);
  };
};

}; // namespace iuic::environment
