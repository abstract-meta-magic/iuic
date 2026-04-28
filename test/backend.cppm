

export module iuic.backend;
import iuic.core;

export namespace iuic::backend {

struct instance {
  virtual ~instance() = default;
  virtual void eval(iuic::context &) = 0;
  virtual units::ui::size get_viewport_size() = 0;
  virtual void set_viewport_size(units::ui::size) = 0;
  static instance &get();
};

}; // namespace iuic::backend
