// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:persist;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
export import :persist.decl;
export import :persist.object;
export import :persist.state;

export namespace iuic::environment {

// TODO : MEMORY LEAK HERE
struct persist {

  persist_object_storage object;

  /*
  struct : private iuic::advance::interface {
    using buff_t = utils::swap_buffers<
        std::unordered_map<units::uid, std::unordered_set<state::value>>, 3>;
    friend persist;
    void attach(units::uid uid, iuic::state::value value) {
      auto [old, cur] = data.get_buffers();

      data.move_forward(uid); // auto check old value

      if (not cur.contains(uid)) {
        cur.insert({uid, {value}});
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
        data.move_forward(uid);
        return cur.at(uid).contains(value);
      } else if (old.contains(uid)) {
        return old.at(uid).contains(value);
      } else {
        return false;
      };
    };

    void update_lifetime(units::uid uid) {
      auto buff = data.get_buffers();
      if (auto node = buff.prev.extract(uid)) {
        std::println("before update old[{}],cur[{}]", buff.prev.size(),
                     buff.current.size());
        buff.current.insert(std::move(node));
        std::println("after update old[{}],cur[{}]", buff.prev.size(),
                     buff.current.size());
      };
      // data.move_forward(uid);
    };

  protected:
    void advance() override {
      auto [old, cur] = data.get_buffers();

      data.swap();
    };

  private:
    buff_t data;
  } state;
  */

  persist_state_storage state;

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
    units::ui::position get_pointer_position() { return pointer_position; };

    units::ui::position get_old_pointer_position() {
      return old_pointer_position;
    };

    void set_pointer_position(units::ui::position ppos) {
      old_pointer_position = std::exchange(pointer_position, ppos);
    };

    units::keycode key_code;

  private:
    units::ui::position old_pointer_position;
    units::ui::position pointer_position;
  } external;

  persist(advance::pool &pool) {
    object.rebind(pool);
    state.rebind(pool);
    machine.rebind(pool);
    external.rebind(pool);
  };
};

}; // namespace iuic::environment
