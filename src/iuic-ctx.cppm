
module;

#include <concepts>
#include <cstddef>
#include <memory_resource>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

export module iuic.core;
export import :base;
import :layout.box;
import :transform;
import :ftc;
import :storage;
import :animator;
import :event;

// import :default

export namespace iuic {

// base stye
constexpr style def_style = []() {
  style res{};

  res.shape.min_size = {120, 240};

  res.shape.margin.top = 20;
  res.shape.margin.left = 30;

  res.background.color = {150, 11, 11, 255};

  return res;
}();

constexpr const auto def_layout = box_layout{};
// constexpr const layout &def_layout = def_layout_val;

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
    template <style = def_style, layout_cpt layout = box_layout,
              typename Call = void>
    void frame(Call call);

    template <style = def_style, layout_cpt layout = box_layout> void frame();

    /*
      Является конечной точкой.Отрисовка текста
    */
    template <style = def_style> void text(std::string_view str);

    /*
      Является конечной точкой.Отрисовка изображения
    */
    template <style = def_style> void image(image_render_data);

    /*
      Пользовательская поверхность.
    */
    template <style = def_style, layout_cpt layout = box_layout,
              typename Call = void>
    void surface(Call call, surface_create_info);

    template <style = def_style, layout_cpt layout = box_layout>
    void surface(surface_create_info);

    // event
    template <KeyAction ka, typename Call = void>
    void event(key_code kc, KeyMod km, Call &&call) {
      // Проверять на правельный тип вызова
      ctx.event_registry.registry_key_event<Call{}>(
          ctx.calc_tree.get_current_id(), kc, km, ka);
    };

    template <KeyAction key, typename Call = void>
    void event(key_code kc, Call &&call) {
      event<key, Call>(kc, KeyMod::None, std::forward<Call>(call));
    };

    template <KeyAction ka, std::invocable<key_event_transfer_data> Call = void>
    void event(key_code kc, KeyMod km, Call &&call, storage_registry_key srk) {
      // Проверять на правельный тип вызова
      ctx.event_registry.registry_key_event<Call{}>(
          ctx.calc_tree.get_current_id(), kc, km, ka, srk);
    };

    template <KeyAction key,
              std::invocable<key_event_transfer_data> Call = void>
    void event(key_code kc, Call &&call, storage_registry_key srk) {
      event<key, Call>(kc, KeyMod::None, std::forward<Call>(call), srk);
    };

    template <PointerAction, typename Call = void> void event(Call){};

    // использовать трансформатор для изменения
    // позиций, размеров и вращения элементов
    // transform.shape
    // transform.position
    void transform();

    const storage &storage;

  private:
    builder(context &ctx_) : ctx{ctx_}, storage{ctx_.storage} {};
    // animator
    context &ctx;
  };

public: // api
  void set_view_size(ui_size sz);

  template <UpdateType = UpdateType::Dynamic, typename Call = void>
  void make(Call call);

  const std::vector<relement> &get_tree();

private:
  void reset();
  // первичное вычисление своего размера
  void self_size();
  // примерное расположение своих детей
  void childs_position();
  // балансировка
  void balancing();
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
  FCTree calc_tree;
  // дерево событий
  tmp_event_registry event_registry{storage};
  // плоский список отрисовки
  std::vector<relement> to_render;
  // ядро построения
  builder b{*this};

  // стелизация точки входа
  style viewport;
};

// Contex Template Impl
template <UpdateType = UpdateType::Dynamic, typename Call = void>
void context::make(Call call) {
  // step 1
  reset();

  // Может его сдеалть частью класса context ?
  style st{.shape{.min_size{view_size}, .max_size{view_size}}};
  // begin построение FTC
  calc_tree.add(st, &def_layout);
  call(b);
  calc_tree.up();
  // end

  self_size();
  childs_position();
  balancing();

  // dop
  build_render_list();
};

// Builder Template Impl
template <style st = def_style, layout_cpt Layout, typename Call = void>
void context::builder::frame(Call call) {

  // bg
  // boreders
  ctx.calc_tree.add(st, &layout::instance<Layout>());
  call(*this);
  ctx.calc_tree.up();
};

template <style st = def_style, layout_cpt Layout>
void context::builder::frame() {

  // bg
  // boreders
  ctx.calc_tree.add(st, &layout::instance<Layout>());
  ctx.calc_tree.up();
};

template <style st = def_style, layout_cpt Layout, typename Call = void>
void context::builder::surface(Call call, surface_create_info sci) {
  ctx.calc_tree.add(st, &layout::instance<Layout>());

  // wrong
  surface_static_info s{};
  surface_render_data sr{s};
  // wrong
  call(*this, sr);
  ctx.calc_tree.up();
}

template <style st = def_style, layout_cpt Layout>
void context::builder::surface(surface_create_info sci) {
  ctx.calc_tree.add(st, &layout::instance<Layout>());

  // wrong
  surface_static_info s{};
  surface_render_data sr{s};
  // wrong
  ctx.calc_tree.up();
}

template <style st = def_style>
void context::builder::text(std::string_view str) {
  ctx.calc_tree.add(st, &layout::instance<box_layout>());
  // WARNING : установить данные для отрисовки текста
  ctx.calc_tree.up();
}

template <style st = def_style>
void context::builder::image(image_render_data ird) {
  ctx.calc_tree.add(st, &layout::instance<box_layout>());
  // WARNING : установить данные для отрисовки текста
  ctx.calc_tree.up();
}
} // namespace iuic
