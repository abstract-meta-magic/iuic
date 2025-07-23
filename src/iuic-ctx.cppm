
module;

#include <variant>

export module iuic.core;
export import :base;
import :ftc;
import :storage;
import :animator;
import :event;

export namespace iuic {

// base stye
constexpr style def_style{.shape{.min_size{200, 100}}};

// WARRNING : Not Impl eat
enum class UpdateType { Dynamic, Static, DirtyFlag };

// build contex and run
// данные можно сохранять в store
// и даже изменять между обновлениями
class context {
public:
  class builder {
    friend context;

    // TOTO : добавить layout's
  public:
    /*
      Базовая форма для всего.Стелизуемый рамка.
    */
    template <style = def_style, UpdateType update = UpdateType::Dynamic,
              typename Call = void>
    void frame(Call call);

    /*
      Является конечной точкой.Отрисовка текста
    */
    template <style = def_style, UpdateType = UpdateType::Dynamic,
              typename Call = void>
    void text(Call call);

    /*
      Является конечной точкой.Отрисовка изображения
    */
    template <style = def_style, UpdateType = UpdateType::Dynamic,
              typename Call = void>
    void image(Call call);

    /*
      Пользовательская поверхность.
    */
    template <style = def_style, UpdateType = UpdateType::Dynamic,
              typename Call = void>
    void surface(Call call, const surface_static_info &);

  private:
    builder(context &ctx_) : ctx{ctx_} {};
    // animator
    context &ctx;
  };

public: // api
  void set_view_size(ui_size sz);

  template <UpdateType = UpdateType::Dynamic, typename Call = void>
  void make(Call call);

  render_list get_tree();

private:
  void reset();
  // первый этап вычислений
  void primary_calculations();
  // балансировка
  void balancing();
  // построение дерева событий на основе FTC
  void build_event_tree();
  // построение списка отрисовки на основе FTC
  void build_render_list();

public:
  // event reciver
  event_reciver event{event_registry};
  // store
  storage storage;

  // animator
  animator animator;

private:
  ui_size view_size;

  // плоское дерево вычислений
  FTC calc_tree;
  // дерево событий
  tmp_event_registry event_registry;
  // плоский список отрисовки
  render_list rl;
  // ядро построения
  builder b{*this};
};

// Contex Template Impl
template <UpdateType = UpdateType::Dynamic, typename Call = void>
void context::make(Call call) {
  // step 1
  reset();

  style st{.shape{.min_size{view_size}, .max_size{view_size}}};
  // построение FTC
  calc_tree.add(st, frame_render_data{{}});
  call(b);
  calc_tree.up();
  // step 2
  primary_calculations();
  balancing();
  // step 3
  // can parallel
  build_event_tree();
  build_render_list();
};

// Builder Template Impl
template <style st = def_style, UpdateType = UpdateType::Dynamic,
          typename Call = void>
void context::builder::frame(Call call) {

  // bg
  // boreders

  ctx.calc_tree.add(st, frame_render_data{st.background});
  call(*this);
  ctx.calc_tree.up();
};

template <style st = def_style, UpdateType = UpdateType::Dynamic,
          typename Call = void>
void context::builder::surface(Call call, const surface_static_info &si) {
  ctx.calc_tree.add(st, surface_render_data{si});

  // wrong
  surface_static_info s{};
  surface_render_data sr{s};
  // wrong
  call(*this, sr);
  ctx.calc_tree.up();
}
} // namespace iuic
