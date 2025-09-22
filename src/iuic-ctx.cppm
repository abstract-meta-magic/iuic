
module;

#include <concepts>
#include <cstddef>
#include <memory_resource>
#include <stack>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

export module iuic.core;
export import :base;
export import :base.color;
import :layout.box;
import :fct;
export import :storage;
import :pseudo_selector;
export import :event;

// import :default

export namespace iuic {

// TOTO : replace
struct pseudo_selector_impl__ final : advanced_pseudo_selector {
  bool is_hovered(uid_t uid) const noexcept override {
    return hovered.contains(uid);
  };

  bool is_active(uid_t uid) const noexcept override {
    return active.contains(uid);
  };

  bool is_focused(uid_t uid) const noexcept override {
    return focused.contains(uid);
  };

  void set_active(uid_t uid) noexcept override { active.insert(uid); };

  void set_focused(uid_t uid) noexcept override { focused.insert(uid); };

  void set_hovered(uid_t uid) noexcept override { hovered.insert(uid); };

  // TODO : future in set<uid_t>
  void unset_focused(uid_t uid) noexcept override { focused.erase(uid); };

  void unset_hovered(uid_t uid) noexcept override { hovered.erase(uid); };

  // TODO : future in set<uid_t>
  void unset_active(uid_t uid) noexcept override { active.erase(uid); };

  void unset_focused() noexcept override { focused.clear(); };

  void unset_active() noexcept override { active.clear(); };

  pseudo_selector_snapshot snapshot() const noexcept override {
    return {hovered, focused, active};
  };

private:
  std::unordered_set<uid_t> hovered;

  std::unordered_set<uid_t> focused;

  std::unordered_set<uid_t> active;
};
// base stye
constexpr style def_style = []() {
  style res{};

  res.shape.min_size = {percent_t{16}, percent_t{24}};

  res.positioning.margin.top = percent_t{4};
  res.positioning.margin.left = upixel_t{30};

  res.shape.border.top = upixel_t{20}, res.shape.border.left = upixel_t{40};

  res.background = color::css::white{};

  return res;
}();

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
      TODO : можно заменить на нешаблонный вызов
    */
    void frame(std::invocable<builder &> auto &&call, const style & = def_style,
               const frame_layout & = box_layout) noexcept;

    void frame(std::invocable<builder &> auto &&call,
               const frame_layout &) noexcept;

    void frame(const style & = def_style,
               const frame_layout & = box_layout) noexcept;

    void frame(const frame_layout &) noexcept;

    /*
      Является конечной точкой.Отрисовка текста
    */
    void text(std::string_view str, const style & = def_style);

    // event
    template <key_event_callback_cpt Call>
    void event(Call &&call, storage_registry_key key = {}) {
      if constexpr (std::invocable<decltype(call), iuic::event_type::key_g>) {
        ctx.event_collector.push(
            key_event{key, {call}, ctx.ctree.current_index(), 0});
      } else {
        ctx.event_collector.push(
            key_event{key, {call}, ctx.ctree.current_index(), uid});
      }
    };

    template <pointer_event_callback_cpt Call>
    void event(Call &&call, storage_registry_key key = {}) {
      ctx.event_collector.push(
          pointer_event{key, {call}, ctx.ctree.current_index(), uid});
    };

    // использовать трансформатор для изменения
    // позиций, размеров и вращения элементов будет добавленно в
    // следующих версиях
    // transform.shape
    // transform.position
    void transform();

    // base uid + str.hash
    uid_t make_uid(const std::string &) const noexcept;

    // base uid + other.uid.hash + str.hash
    uid_t make_uid(uid_t, const std::string &) const noexcept;

    // base uid + ptr.hash
    template <typename T>
      requires std::is_pointer_v<T>
    uid_t make_uid(T ptr) const noexcept {
      return __make_uid_from_ptr(reinterpret_cast<const void *>(ptr));
    };

    template <typename T, typename... ARGS>
    uid_t make_uid(T(ptr)(ARGS...)) const noexcept {
      return __make_uid_from_ptr(reinterpret_cast<const void *>(ptr));
    };

    void apply_uid(uid_t uid) noexcept;

    mutable_storage &storage;

  private:
    uid_t __make_uid_from_ptr(const void *ptr) const noexcept;

    uid_t __make_base_uid() const noexcept;

    void __prev() noexcept;

    void __post() noexcept;

    builder(context &ctx_) : ctx{ctx_}, storage{ctx_.storage} {};
    // animator
    context &ctx;
    std::vector<size_t> id;
    std::stack<size_t, std::vector<size_t>> seed;
    uid_t uid{0};
  };

public: // api
  void set_view_size(ui_size sz);

  template <typename Call = void> void make(Call call);

  // TODO : rename
  const std::vector<relement> &get_tree();

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

public:
  managed_storage storage;
  // event reciver
  event_reciver event{storage, selector};
  // store
private:
  pseudo_selector_impl__ selector{};
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

  proccess_measure();
  proccess_arrange();
  proccess_position();

  // ctree.print_tree();

  apply_event_hit_surface(event, event_collector.build_pack(ctree));
  // dop
  build_render_list();
};

// --- Builder Template Impl ---
void context::builder::frame(std::invocable<context::builder &> auto &&call,
                             const style &style,
                             const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);

  builder::__prev();
  call(*this);
  builder::__post();

  ctx.ctree.up();
};

void context::builder::frame(std::invocable<context::builder &> auto &&call,
                             const frame_layout &layout) noexcept {
  frame(std::forward<decltype(call)>(call), def_style, layout);
};

void context::builder::frame(const style &style,
                             const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);
  ctx.ctree.up();
};

void context::builder::frame(const frame_layout &layout) noexcept {
  frame(def_style, layout);
};

void context::builder::text(std::string_view str, const style &st) {
  ctx.ctree.add(st, &box_layout);
  // WARNING : установить данные для отрисовки текста
  ctx.ctree.up();
}
} // namespace iuic
