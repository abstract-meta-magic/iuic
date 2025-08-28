
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
import :uid.factory;
import :layout.box;
import :transform;
import :fct;
import :storage;
import :animator;
export import :event;

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
      Пользовательская поверхность. Будет добавленно в v0.2
    */
    template <style = def_style, layout_cpt layout = box_layout,
              typename Call = void>
    void surface(Call call, surface_create_info);

    template <style = def_style, layout_cpt layout = box_layout>
    void surface(surface_create_info);

    // event
    template <event_callback_cpt Call>
    void event(Call &&call, storage_registry_key key = {}) {
      ctx.event_collector.push({key, {call}}, ctx.ctree.index_at_last());
    };

    // использовать трансформатор для изменения
    // позиций, размеров и вращения элементов будет добавленно в
    // следующих версиях
    // transform.shape
    // transform.position
    void transform();

    // base uid + str.hash
    uid_t make_uid(const std::string &) const noexcept;

    // base uid + other.uid.hash
    uid_t make_uid(uid_t) const noexcept;

    // base uid + other.uid.hash + str.hash
    uid_t make_uid(uid_t, const std::string &) const noexcept;

    // base uid + ptr.hash
    template <typename T>
      requires std::is_pointer_v<T>
    uid_t make_uid(T ptr) const noexcept {
      return __make_uid_from_ptr(static_cast<const void *>(ptr));
    };

    void apply_uid(uid_t uid) noexcept;

    const storage &storage;

  private:
    uid_t __make_uid_from_ptr(const void *ptr) const noexcept;

    uid_t __make_base_uid() const noexcept;

    builder(context &ctx_) : ctx{ctx_}, storage{ctx_.storage} {};
    // animator
    context &ctx;
    uid::factory factory;
  };

public: // api
  void set_view_size(ui_size sz);

  template <typename Call = void> void make(Call call);

  // TODO : rename
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
  event_reciver event;
  // store
  storage storage;

  // animator
  animator animator;

private:
  // плоское дерево вычислений
  FCTree ctree;
  // дерево событий
  event_collector event_collector;
  // плоский список отрисовки
  std::vector<relement> to_render;
  // ядро построения
  builder b{*this};
};

template <typename T>
concept ctx_builder_cpt = std::same_as<std::remove_cvref<T>, context::builder>;

// Contex Template Impl
template <typename Call = void> void context::make(Call call) {
  // step 1
  reset();

  // построение FCT
  call(b);

  self_size();
  childs_position();
  balancing();

  // ctree.print_tree();

  apply_event_hit_surface(event, event_collector.build_surface(ctree));
  // dop
  build_render_list();
};

// Builder Template Impl
template <style st = def_style, layout_cpt Layout, typename Call = void>
void context::builder::frame(Call call) {

  // bg
  // boreders
  ctx.ctree.add(st, &layout::instance<Layout>());
  call(*this);
  ctx.ctree.up();
};

template <style st = def_style, layout_cpt Layout>
void context::builder::frame() {

  // bg
  // boreders
  ctx.ctree.add(st, &layout::instance<Layout>());
  ctx.ctree.up();
};

template <style st = def_style, layout_cpt Layout, typename Call = void>
void context::builder::surface(Call call, surface_create_info sci) {
  ctx.ctree.add(st, &layout::instance<Layout>());

  // wrong
  surface_static_info s{};
  surface_render_data sr{s};
  // wrong
  call(*this, sr);
  ctx.ctree.up();
}

template <style st = def_style, layout_cpt Layout>
void context::builder::surface(surface_create_info sci) {
  ctx.ctree.add(st, &layout::instance<Layout>());

  // wrong
  surface_static_info s{};
  surface_render_data sr{s};
  // wrong
  ctx.ctree.up();
}

template <style st = def_style>
void context::builder::text(std::string_view str) {
  ctx.ctree.add(st, &layout::instance<box_layout>());
  // WARNING : установить данные для отрисовки текста
  ctx.ctree.up();
}

template <style st = def_style>
void context::builder::image(image_render_data ird) {
  ctx.ctree.add(st, &layout::instance<box_layout>());
  // WARNING : установить данные для отрисовки картинки
  ctx.ctree.up();
}
} // namespace iuic
