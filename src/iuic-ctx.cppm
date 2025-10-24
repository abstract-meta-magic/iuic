
module;

#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <print>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

export module iuic.core;
export import :base;
export import :base.color;
export import :layout.def;
import :layout.frame.box;
import :layout.text.box;
import :fct;
export import :storage.def;
export import :storage.object;
export import :storage.text;
export import :text.token;
export import :text.present;
export import :state;
export import :state.transition;
export import :event;

// import :default

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

// WARRNING : Not Impl eat
enum class UpdateType { Dynamic, Static, DirtyFlag };

// build contex and run
// данные можно сохранять в store
// и даже изменять между обновлениями
class context {
public: // public forward decl
  struct builder;

  struct transition_utils {
    transition_utils(context &ctx_, size_t index_, time_t time_, uid_t uid_,
                     ork_t ork_ = 0, trk_t trk_ = 0)
        : time_point{time_}, uid{uid_}, ork{ork_}, trk{trk_},
          style{ctx_, index_}, storage{ctx_} {};

    const time_t time_point;
    const uid_t uid;
    const ork_t ork;
    const trk_t trk;

    struct style_interface {
      style_interface(context &ctx, std::size_t index);

      void override(style::shape &&);

      void override(style::decoration &&);

      void override(style::transform &&);

    private:
      context &ctx;
      size_t index;
    } style;

    struct storage_interface {
      storage_interface(context &);
      mutable_object_storage &object;
      mutable_text_storage &text;
    } storage;
  };

private: // builder.def
  struct builder_base {
    struct unit {
      uid_t uid{0};
      size_t index{0};
    };
    builder_base(context &ctx_) : ctx{ctx_} {
      static std::string root_uid{"root-uid-hash-str-4467532667"};
      uids.push(unit{hash::make(4474444, root_uid.c_str(), root_uid.size())});
    };

  protected:
    void __prev() noexcept { uids.push(uids.top()); };

    void __post() noexcept {
      ctx.ctree.current().get_info().uid = uids.top().uid;
      uids.pop();
    };

  protected: // builder unit stack
    context &ctx;
    std::stack<unit> uids;
  };

  struct builder_unit_interface : protected virtual builder_base {
    builder_unit_interface(builder_base &&bb) : builder_base{bb} {};

    /*
      Базовая форма для всего.Стелизуемый рамка.
      TODO : можно заменить на нешаблонный вызов
    */
    void frame(std::invocable<builder &> auto &&call, style::ref = def_style,
               const frame_layout & = box_layout) noexcept;

    void frame(std::invocable<builder &> auto &&call,
               const frame_layout &) noexcept;

    void frame(style::ref = def_style,
               const frame_layout & = box_layout) noexcept;

    void frame(const frame_layout &) noexcept;

    /*
      Является конечной точкой.Отрисовка текста
    */
    void text(text_registry_key, style::ref = def_style,
              const text_layout & = text_def_layout);
  };

  struct builder_order_interface : protected virtual builder_base {
    builder_order_interface(builder_base &&bb) : builder_base{bb} {}

    void group(std::uint16_t value) {
      ctx.ctree.current().get_info().order.group = value;
    };

    void up() { ctx.ctree.current().get_info().order.priority += 1; };

    void set(std::uint16_t value) {
      ctx.ctree.current().get_info().order.priority += value;
    };
  };

  struct builder_policy_interface : protected virtual builder_base {
    builder_policy_interface(builder_base &&bb) : builder_base{bb} {};

    void hovered(policy::hovered p) {
      ctx.ctree.current().get_info().hovered_p = p;
    };

    void event(policy::event p) { ctx.ctree.current().get_info().event_p = p; };
  };

  struct builder_uid_interface : protected virtual builder_base {
    static inline constexpr uid::anchor default_anchor{};

    builder_uid_interface(builder_base &&bb) : builder_base{bb} {};

    uid_t make(const std::string &str,
               const uid::anchor &anchor = default_anchor) const noexcept {
      std::stringstream ss;
      ss << str;
      ss << &default_anchor;

      auto hash_string = ss.str();
      return hash::make(hash_string.c_str(), hash_string.length());
    };

    uid_t make(policy::shared, const std::string &str,
               const uid::anchor &anchor = default_anchor) {
      std::stringstream ss;
      ss << str;
      ss << &default_anchor;
      ss << ctx.ctree.current().get_hierarchy().parent;

      auto hash_string = ss.str();
      return hash::make(hash_string.c_str(), hash_string.length());
    };

    uid_t make(policy::unique, const std::string &str,
               const uid::anchor &anchor = default_anchor) {
      std::stringstream ss;
      ss << str;
      ss << &default_anchor;
      ss << ctx.ctree.current_index(); // error
      ss << ctx.ctree.current().get_hierarchy().parent;

      auto hash_string = ss.str();
      return hash::make(hash_string.c_str(), hash_string.length());
    };

    uid_t make(policy::indexed, const std::string &str,
               const uid::anchor &anchor = default_anchor) {
      std::stringstream ss;
      ss << str;
      ss << &default_anchor;
      ss << ctx.ctree.current().get_hierarchy().parent;
      ss << ++uids.top().index;

      auto hash_string = ss.str();
      return hash::make(hash_string.c_str(), hash_string.length());
    };

    void branch(uid_t uid) { uids.top().uid = uid; };

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

    void operator()(custom_event_callback_cpt auto &&call, ork_t ork = 0,
                    trk_t trk = 0) {
      // ...
    };

    // event
    template <event_callback_cpt Call>
    void attach(Call &&call, object_registry_key ork = 0,
                text_registry_key trk = 0) {
      ctx.event_collector.push(revent{std::forward<Call>(call), ork, trk,
                                      ctx.ctree.current_index()});
    };

    template <event_callback_cpt Call>
    void attach(Call &&call, text_registry_key trk) {
      attach<Call>(std::forward<Call>(call), 0, trk);
    }

    void attach(custom_event_callback_cpt auto &&call, ork_t ork = 0,
                trk_t trk = 0) {};
  };

  struct builder_style_interface : protected virtual builder_base {
    builder_style_interface(builder_base &&bb) : builder_base{bb} {};

    void override(style::shape &&shape) {
      // allocate tmp
      auto ptr = ctx.frame_memory<style::shape>();

      new (ptr) style::shape{std::move(shape)};

      ctx.ctree.current().get_info().style.override(ptr);
    };

    void override(style::transform &&transform) {
      auto ptr = ctx.frame_memory<style::transform>();

      new (ptr) style::transform{std::move(transform)};

      ctx.ctree.current().get_info().style.override(ptr);
    };

    void override(style::decoration &&decoration) {
      auto ptr = ctx.frame_memory<style::decoration>();

      new (ptr) style::decoration{std::move(decoration)};

      ctx.ctree.current().get_info().style.override(ptr);
    };
  };

  struct builder_state_interface : protected virtual builder_base {
    builder_state_interface(builder_base &&bb) : builder_base{std::move(bb)} {};

    // TODO : use uid
    void
    transition(uid_t uid, pseudo_state from, pseudo_state to,
               std::convertible_to<state_transition (*)(transition_utils)> auto
                   &&call) {
      auto actual_state = ctx.state.pseudo(uid);

      if (actual_state.from() == from && actual_state.to() == to &&
          to != pseudo_state::null()) {
        auto tr = call(
            {ctx, ctx.ctree.current_index(), actual_state.time_point(), uid});
        tr.from = from;
        tr.to = to;
        tr.uid = uid;
        ctx.state_tr.apply(ctx.ctree.current_index(), std::move(tr));
      }
    };

    auto pseudo(uid_t uid) { return ctx.state.pseudo(uid); };

    void pseudo_init_value(uid_t uid, pseudo_state state) {
      ctx.state.pseudo_init_value(uid, state);
    }

    bool hovered(uid_t uid) { return ctx.state.hovered(uid); };
  };

public:
  struct builder final : public virtual builder_base,
                         private builder_unit_interface,
                         private builder_uid_interface,
                         private builder_policy_interface,
                         private builder_storage_interface,
                         private builder_event_interface,
                         private builder_style_interface,
                         private builder_state_interface {
    builder(builder_base &&bb) noexcept
        : builder_base{bb}, builder_unit_interface{std::move(bb)},
          builder_uid_interface{std::move(bb)},
          builder_policy_interface{std::move(bb)},
          builder_storage_interface{std::move(bb)},
          builder_event_interface{std::move(bb)},
          builder_style_interface{std::move(bb)},
          builder_state_interface{std::move(bb)} {};
    builder_unit_interface &unit{*this};
    builder_uid_interface &uid{*this};
    builder_policy_interface &policy{*this};
    builder_storage_interface &storage{*this};
    builder_event_interface &event{*this};
    builder_style_interface &style{*this};
    builder_state_interface &state{*this};

    builder(const builder &) = delete;
    builder &operator=(const builder &) = delete;
    builder(builder &&) = delete;
    builder &operator=(builder &&) = delete;
  };

public: // api
  void set_view_size(ui_size sz);

  template <typename Call = void> void make(Call call);

  // TODO : rename to blueprint
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

  event_reciver event{object, text, state};

private:
  template <typename T> T *frame_memory() {
    // TODO : wrap throw ?
    return static_cast<T *>(frame_resource__.allocate(sizeof(T), alignof(T)));
  };

private:
  managed_state_holder state{};
  state_transition_scheduler state_tr{state};

  text::text_present_aggregator tpa{};
  // плоское дерево вычислений
  FCTree ctree;
  // дерево событий
  event_collector event_collector;
  // плоский список отрисовки
  std::vector<relement> to_render;
  // ядро построения
  builder b{builder_base{*this}};

  std::byte frame_memory__[1024 * 1024 * 2];
  std::pmr::monotonic_buffer_resource frame_resource__{frame_memory__,
                                                       sizeof(frame_memory__)};
};

template <typename T>
concept ctx_builder_cpt = std::same_as<std::remove_cvref<T>, context::builder>;

// Contex Template Impl
template <typename Call = void> void context::make(Call call) {
  // step 1
  reset();

  // построение FCT
  call(b);

  state_tr.process();

  proccess_measure();
  proccess_arrange();
  proccess_position();

  // ctree.print_tree();

  apply_event_pack__(event, event_collector.build_pack(ctree));
  // dop
  build_render_list();
};

// --- Builder Template Impl ---

void context::builder_unit_interface::frame(
    std::invocable<context::builder &> auto &&call, style::ref style,
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
    style::ref style, const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);
  ctx.ctree.up();
};

void context::builder_unit_interface::frame(
    const frame_layout &layout) noexcept {
  frame(def_style, layout);
};

void context::builder_unit_interface::text(text_registry_key key, style::ref st,
                                           const text_layout &layout) {
  ctx.ctree.add(st, &layout);

  ctx.tpa.attach_present(key, ctx.ctree.current_index());
  // WARNING : установить данные для отрисовки текста
  ctx.ctree.up();
}

context::transition_utils::storage_interface::storage_interface(context &ctx)
    : object{ctx.object}, text{ctx.text} {};

context::transition_utils::style_interface::style_interface(context &ctx_,
                                                            std::size_t index_)
    : ctx{ctx_}, index{index_} {};

void context::transition_utils::style_interface::override(
    style::decoration &&style) {
  auto ptr = new (ctx.frame_resource__.allocate(sizeof(style::decoration),
                                                alignof(style::decoration)))
      style::decoration{std::move(style)};
  ctx.ctree.at(index).get_info().style.override(ptr);
};

void context::transition_utils::style_interface::override(
    style::shape &&style) {
  auto ptr = new (ctx.frame_resource__.allocate(sizeof(style::shape),
                                                alignof(style::shape)))
      style::shape{std::move(style)};

  ctx.ctree.at(index).get_info().style.override(ptr);
};

void context::transition_utils::style_interface::override(
    style::transform &&style) {
  auto ptr = new (ctx.frame_resource__.allocate(sizeof(style::transform),
                                                alignof(style::transform)))
      style::transform{std::move(style)};

  ctx.ctree.at(index).get_info().style.override(ptr);
};
/*
struct ui_scheme_base {
// modify
protected:
std::vector<relement> elements;
};

struct ui_scheme : protected ui_scheme_base {

ui_scheme &operator=(const ui_scheme_base &);
ui_scheme &operator=(ui_scheme_base &&);

// другие интересные функций

void foreach (std::invocable<const celement &> auto &&fn);
};

void test(ui_scheme &sh) {

ui_scheme_base base;

sh = std::move(base);
}
*/
} // namespace iuic
