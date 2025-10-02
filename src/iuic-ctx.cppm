
module;

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

export module iuic.core;
export import :base;
export import :base.color;
import :layout.box;
import :fct;
export import :storage.def;
export import :storage.object;
export import :storage.text;
export import :state;
export import :event;

// import :default

export namespace iuic {

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
public: // public forward decl
  struct builder;

private: // builder.def
  struct builder_base {
    builder_base(context &ctx_) : ctx{ctx_} {
      static std::string root_uid{"root-uid-hash-str-4467532667"};
      uids.push(hash::make(4474444, root_uid.c_str(), root_uid.size()));
    };

  protected:
    void __prev() noexcept { uids.push(uids.top()); };

    void __post() noexcept { uids.pop(); };

  protected: // builder unit stack
    context &ctx;
    std::stack<uid_t> uids;
  };

  struct builder_unit_interface : protected virtual builder_base {
    builder_unit_interface(builder_base &&bb) : builder_base{bb} {};

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
    void text(text_registry_key, const style & = def_style);
  };

  struct builder_order_interface : protected virtual builder_base {
    builder_order_interface(builder_base &&bb) : builder_base{bb} {}

    void group(std::uint16_t value) {
      ctx.ctree.current().get_order().group = value;
    };

    void up() { ctx.ctree.current().get_order().priority += 1; };

    void set(std::uint16_t value) {
      ctx.ctree.current().get_order().priority += value;
    };
  };

  struct builder_policy_interface : protected virtual builder_base {
    builder_policy_interface(builder_base &&bb) : builder_base{bb} {};

    void hovered(policy::hovered p) { ctx.ctree.current().set_policy(p); };

    void event(policy::event p) { ctx.ctree.current().set_policy(p); };
  };

  struct builder_uid_interface : protected virtual builder_base {
    builder_uid_interface(builder_base &&bb) : builder_base{bb} {};
    // base uid + str.hash
    uid_t make(const std::string &str) const noexcept {
      return hash::make(uids.top(), str.c_str(), str.size());
    };

    // base uid + other.uid.hash + str.hash
    uid_t make(uid_t uid, const std::string &str) const noexcept {
      return hash::make(uid, str.c_str(), str.size());
    };

    // base uid + ptr.hash
    template <typename T>
      requires std::is_pointer_v<T>
    uid_t make(T ptr) const noexcept {
      return __make_uid_from_ptr(reinterpret_cast<const void *>(ptr));
    };

    template <typename T, typename... ARGS>
    uid_t make(T(ptr)(ARGS...)) const noexcept {
      return __make_uid_from_ptr(reinterpret_cast<const void *>(ptr));
    };

    void branch(uid_t uid) { uids.top() = uid; };

  private:
    uid_t __make_uid_from_ptr(const void *ptr) const noexcept {
      auto str = std::to_string((size_t)ptr);
      return hash::make(str.c_str(), str.size());
    };
  };

  struct builder_storage_interface : protected virtual builder_base {
    builder_storage_interface(builder_base &&bb) : builder_base{bb} {};
    mutable_object_storage &object{ctx.object};
    mutable_text_storage &text{ctx.text};
  };

  struct builder_event_interface : protected virtual builder_base {
    builder_event_interface(builder_base &&bb) : builder_base{bb} {};

    void operator()(event_callback_cpt auto &&call, object_registry_key ork = 0,
                    text_registry_key trk = 0) {
      attach(std::forward<decltype(call)>(call), ork, trk);
    };

    void operator()(event_callback_cpt auto &&call, text_registry_key trk) {
      attach(std::forward<decltype(call)>(call), trk);
    };

    // event
    template <event_callback_cpt Call>
    void attach(Call &&call, object_registry_key ork = 0,
                text_registry_key trk = 0) {
      ctx.event_collector.push(revent{std::forward<Call>(call), ork, trk,
                                      ctx.ctree.current_index(), 0});
    };

    template <event_callback_cpt Call>
    void attach(Call &&call, text_registry_key trk) {
      attach<Call>(std::forward<Call>(call), 0, trk);
    }
  };

public:
  struct builder final : public virtual builder_base,
                         private builder_unit_interface,
                         private builder_uid_interface,
                         private builder_policy_interface,
                         private builder_storage_interface,
                         private builder_event_interface {
    builder(builder_base &&bb) noexcept
        : builder_base{bb}, builder_unit_interface{std::move(bb)},
          builder_uid_interface{std::move(bb)},
          builder_policy_interface{std::move(bb)},
          builder_storage_interface{std::move(bb)},
          builder_event_interface{std::move(bb)} {};
    builder_unit_interface &unit{*this};
    builder_uid_interface &uid{*this};
    builder_policy_interface &policy{*this};
    builder_storage_interface &storage{*this};
    builder_event_interface &event{*this};
    const_state_holder &state{ctx.state};

    builder(const builder &) = delete;
    builder &operator=(const builder &) = delete;
    builder(builder &&) = delete;
    builder &operator=(builder &&) = delete;
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
  managed_object_storage object;

  managed_text_storage text;
  // event reciver
  event_reciver event{object, text, state};
  // store

  // context() noexcept;

private:
  managed_state_holder state{};
  // плоское дерево вычислений
  FCTree ctree;
  // дерево событий
  event_collector event_collector;
  // плоский список отрисовки
  std::vector<relement> to_render;
  // ядро построения
  builder b{builder_base{*this}};
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

void context::builder_unit_interface::frame(
    std::invocable<context::builder &> auto &&call, const style &style,
    const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);

  this->__prev();
  call((builder &)*this);
  this->__post();

  ctx.ctree.up();
};

void context::builder_unit_interface::frame(
    std::invocable<context::builder &> auto &&call,
    const frame_layout &layout) noexcept {
  frame(std::forward<decltype(call)>(call), def_style, layout);
};

void context::builder_unit_interface::frame(
    const style &style, const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);
  ctx.ctree.up();
};

void context::builder_unit_interface::frame(
    const frame_layout &layout) noexcept {
  frame(def_style, layout);
};

void context::builder_unit_interface::text(text_registry_key key,
                                           const style &st) {

  ctx.ctree.add(st, &box_layout);
  // WARNING : установить данные для отрисовки текста
  ctx.ctree.up();
}

} // namespace iuic
