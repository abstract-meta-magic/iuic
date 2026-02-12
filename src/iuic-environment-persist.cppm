

export module iuic.core:environment.persist;
import std;
import iuic.underlying;
import iuic.state;
import :style;

namespace iuic::environment {

struct persist {
  state::machine::instance *get_machine(units::uid);

  erasure::visited::as_mutable get_object(units::uid);

  bool has_state(units::uid);

  units::ui::size get_viewport_size();

  units::ui::position get_pointer_position();

  // get_momory_model();
};

}; // namespace iuic::environment
