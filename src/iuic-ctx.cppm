
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
import :computing.tree;
export import :scheme;
export import :scheme.builder;
export import :policy;
export import :storage.def;
export import :storage.object;
export import :storage.text;
export import :text.font;
export import :text.token;
export import :text.present;
export import :text.fontset;
export import :style;
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
  builder(const builder &) = delete;
  builder &operator=(const builder &) = delete;
  builder(builder &&) = delete;
  builder &operator=(builder &&) = delete;
};

public: // api
void set_view_size(ui_size sz);

template <typename Call = void> void make(Call call);

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
text::fontslot font;

managed_object_storage object;

managed_text_storage text;

event_reciver event{object, text, state};

scheme::explorer scheme{scheme::incomplete{}};

private:
template <typename T> T *frame_memory() {
  // TODO : wrap throw ?
  return static_cast<T *>(frame_resource__.allocate(sizeof(T), alignof(T)));
};

private:
managed_state_holder state{};
state_transition_scheduler state_tr{state};

text::present::aggregator tpa{};
// плоское дерево вычислений
computing::tree ctree;
// дерево событий
event_collector event_collector;
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

void context::builder_element_interface::frame(
    std::invocable<context::builder &> auto &&call, style::ref style,
    const frame_layout &layout) noexcept {
  frame(ctx.ctree.current()->get_info().uid, std::forward<decltype(call)>(call),
        style, layout);
};

void context::builder_element_interface::frame(
    std::invocable<context::builder &> auto &&call,
    const frame_layout &layout) noexcept {
  frame(ctx.ctree.current()->get_info().uid, std::forward<decltype(call)>(call),
        def_style, layout);
};

void context::builder_element_interface::frame(
    style::ref style, const frame_layout &layout) noexcept {
  frame(ctx.ctree.current()->get_info().uid, layout);
}
void context::builder_element_interface::frame(
    const frame_layout &layout) noexcept {
  frame(def_style, layout);
};

void context::builder_element_interface::frame(
    uid_t uid, std::invocable<context::builder &> auto &&call, style::ref style,
    const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);
  ctx.ctree.current()->get_info().uid = uid;

  this->__prev();
  call((builder &)*this);
  this->__post();

  ctx.ctree.up();
};

void context::builder_element_interface::frame(
    uid_t uid, std::invocable<context::builder &> auto &&call,
    const frame_layout &layout) noexcept {
  frame(uid, std::forward<decltype(call)>(call), def_style, layout);
};

void context::builder_element_interface::frame(
    uid_t uid, style::ref style, const frame_layout &layout) noexcept {
  ctx.ctree.add(style, &layout);
  ctx.ctree.current()->get_info().uid = uid;
  ctx.ctree.up();
};

void context::builder_element_interface::frame(
    uid_t uid, const frame_layout &layout) noexcept {
  frame(uid, def_style, layout);
};

void context::builder_element_interface::text(text_registry_key key,
                                              style::ref st,
                                              const text_layout &layout) {
  ctx.ctree.add(st, &layout);

  // wrong ctx.tpa.attach_present(key, ctx.ctree.current_index());
  // WARNING : установить данные для отрисовки текста
  ctx.ctree.up();
}

void context::builder_element_interface::text(text::token &&token,
                                              style::ref st,
                                              const text_layout &layout) {
  ctx.ctree.add(st, &layout);

  text::token::sequence sq{};

  auto *mem = ctx.frame_memory<text::token>();

  new (mem) text::token{std::move(token)};

  ctx.tpa.reserve_present(ctx.ctree.current_index(),
                          text::token::sequence{mem, mem + 1}); // like span

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
  ctx.ctree.at(index)->get_info().style.override(ptr);
};

void context::transition_utils::style_interface::override(
    style::shape &&style) {
  auto ptr = new (ctx.frame_resource__.allocate(sizeof(style::shape),
                                                alignof(style::shape)))
      style::shape{std::move(style)};

  ctx.ctree.at(index)->get_info().style.override(ptr);
};

void context::transition_utils::style_interface::override(
    style::transform &&style) {
  auto ptr = new (ctx.frame_resource__.allocate(sizeof(style::transform),
                                                alignof(style::transform)))
      style::transform{std::move(style)};

  ctx.ctree.at(index)->get_info().style.override(ptr);
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
