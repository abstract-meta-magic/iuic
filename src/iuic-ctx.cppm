
module;

export module iuic.core;
import std;
export import :base;
export import :base.color;
export import :layout.def;
import :layout.frame.box;
import :layout.text.box;
export import :kernel;
export import :scheme;
export import :scheme.builder;
export import :policy;
export import :text.font;
export import :text.token;
export import :text.present;
export import :text.fontset;
export import :style;
export import :state;
export import :state.transition;
export import :event;

export namespace iuic {

// base stye
constexpr style::decl def_style = []() {
  style::decl res{};

  res.shape.min_size = {percent_t{16}, percent_t{24}};

  res.shape.margin.top = percent_t{4};
  res.shape.margin.left = upixel_t{30};

  res.shape.border.top = upixel_t{20}, res.shape.border.left = upixel_t{40};

  res.decoration.background = color::css::white{};

  return res;
}();

// future concept

class context {
public: // api
  void set_view_size(ui_size sz);

  void make(scheme::builder_block_cpt auto &&call);

private:
  void reset();
  // первичное вычисление своего размера
  void proccess_measure();
  // примерное расположение своих детей
  void proccess_arrange();
  // балансировка
  void proccess_position();
  // построение списка отрисовки на основе FTC
  void build_render_list();

private:
  /* Концепт для поддержки __attribute__((weak))
  template<kernel_instance base,kernel_instance ...alt> context;

  auto instance__kernel__() {
    if constexpr (sizeof...(alt) > 0) {
      return base_kernel_instance ? base_kernel_instance()
                                  : ((alt ? alt() : nullptr) || ...);
    } else {
      return base_kernel_instance();
    }
  };
  */

  text::present::aggregator tpa{};
  // плоское дерево вычислений
  std::unique_ptr<kernel::hardware> kernel{kernel::default_kernel()};

  event_collector event_collector;

  scheme::builder b{scheme::builder_base{*kernel, event_collector, b}};

public:
  text::fontslot font;

  event_reciver event{*kernel};

  scheme::explorer scheme{scheme::incomplete{}};
};

// Contex Template Impl
void context::make(scheme::builder_block_cpt auto &&call) {
  // step 1
  reset();

  // построение FCT
  call(b);

  proccess_measure();
  proccess_arrange();

  apply_event_pack__(event, event_collector.build_pack(*kernel));
  // dop
  build_render_list();
};

} // namespace iuic
