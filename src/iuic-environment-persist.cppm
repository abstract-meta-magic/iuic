

export module iuic.core:environment.persist;
import std;
import iuic.underlying;
import iuic.state;
import :style;

namespace iuic::environment {

struct persist {
  struct : private iuic::advance::interface {
    friend persist;
    enum object_state {
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

    erasure::visited::as_mutable get(units::uid);

    void reserve(units::uid);

    template <typename T> void reserve(units::uid);

    void construct(units::uid, auto &&call);

    void destruct(units::uid);

    void update_lifetime(units::uid);

    object_state state(units::uid);
  } object;

  struct : private iuic::advance::interface {
    friend persist;
    void attach(units::uid, iuic::state::value);

    void detach(units::uid, iuic::state::value);

    bool has(units::uid, iuic::state::value);

  private:
    // map
  } state;

  struct : private iuic::advance::interface {
    friend persist;
  } machine;

  struct : private iuic::advance::interface {
    friend persist;
    units::ui::size get_viewport_size() { return viewport_size; };

    void set_viewport_size(units::ui::size vp) { viewport_size = vp; };

    units::ui::position get_pointer_position() { return pointer_position; };

    // TODO : mb? get_old_pointer_position();

    void set_pointer_position(units::ui::position ppos) {
      old_pointer_position = std::exchange(pointer_position, ppos);
    };

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

void heh(persist &env, units::uid uid) {
  env.object.get(uid);
  env.state.has(uid, iuic::state::base::hovered);
  env.external.get_pointer_position();
};

}; // namespace iuic::environment
