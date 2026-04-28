

export module iuic.backend;
import iuic.core;

namespace iuic::backend {

struct instance {
  virtual ~instance() = default;
  virtual void eval(iuic::context &) = 0;
};

instance &get_instance();

}; // namespace iuic::backend
