

export module iuic.core:environment.persist;
import std;
import iuic.underlying;
import iuic.state;
import :style;

namespace iuic::environment {

struct persist {
  struct {
    void get(units::uid);

    void construct(units::uid, auto &&call);

    void destruct(units::uid);

    void update_lifetime(units::uid);

    void state(units::uid);
  } object;

  struct {
    void attach(units::uid, iuic::state::value);

    void detach(units::uid, iuic::state::value);

    bool has(units::uid, iuic::state::value);

  private:
    // map
  } state;

  struct {
  } machine;

  struct {
    units::ui::size get_viewport_size();

    void set_viewport_size(units::ui::size);

    units::ui::position get_pointer_position();

    // TODO : mb? get_old_pointer_position();

    void set_pointer_position(units::ui::position);
  } external;
};

void heh(persist &env, units::uid uid) {
  env.object.get(uid);
  env.state.has(uid, iuic::state::base::hovered);
  env.external.get_pointer_position();
};

}; // namespace iuic::environment
