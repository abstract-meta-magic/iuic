
export module iuic.core:environment.tmp;
import iuic.underlying;
import iuic.state;
import :machine.dispatcher;
import :kernel;
import :key_code;

namespace iuic::environment {
struct tmp {
  void text();
  void style();
  void events();
  void polices();
};
}; // namespace iuic::environment
