

module;

export module iuic.core:scheme.builder;
import std;
import :base;
import :kernel;
import :storage.object;
import :storage.text;
import :text.token;
import :text.buff;
import :layout.frame.box;
import :layout.text.box;
import :event;
import :scheme;

export namespace iuic::scheme {

struct builder;

template <typename T>
concept builder_block_cpt = std::invocable<T, builder &>;

constexpr style::decl def_style{};

struct memory {
  // object  - storage
  // text    - storage
  // persist - memory
  // tmp     - memory
};

struct builder_base {
  struct unit_t {
    uid_t uid{0};
    std::size_t index{0};
  };

  builder_base(kernel::root &ctx_, event_collector &collector_,
               builder &builder_)
      : kernel{ctx_}, collector{collector_}, builder{builder_} {
    static std::string root_uid{"root-uid-hash-str-4467532667"};
    unit.push(unit_t{.uid = kernel.hash(std::as_bytes(std::span(root_uid)))});
  };

protected: // builder unit stack
  kernel::root &kernel;
  event_collector &collector;
  builder &builder;
  // tmp memory
  std::stack<unit_t> unit;
};

struct builder_element_interface : protected virtual builder_base {
  builder_element_interface(builder_base &&bb) : builder_base{bb} {};

  /*
    Базовая форма для всего.Стелизуемый рамка.
    TODO : можно заменить на нешаблонный вызов
  */
  void frame(builder_block_cpt auto &&call, style::ref = def_style,
             const frame_layout & = box_layout) noexcept;

  void frame(builder_block_cpt auto &&call, const frame_layout &) noexcept;

  void frame(style::ref = def_style,
             const frame_layout & = box_layout) noexcept;

  void frame(const frame_layout &) noexcept;

  void frame(uid_t uid, builder_block_cpt auto &&call, style::ref = def_style,
             const frame_layout & = box_layout) noexcept;

  void frame(uid_t uid, builder_block_cpt auto &&call,
             const frame_layout &) noexcept;

  void frame(uid_t uid, style::ref = def_style,
             const frame_layout & = box_layout) noexcept;

  void frame(uid_t uid, const frame_layout &) noexcept;

  /*
    Является конечной точкой.Отрисовка текста
  */
  void text(iuic::text::token &&, style::ref = def_style,
            const text_layout & = text_def_layout);

  // in frame
  void text(const iuic::text::token &, style::ref = def_style,
            const text_layout & = text_def_layout);
};

struct builder_order_interface : protected virtual builder_base {
  builder_order_interface(builder_base &&bb) : builder_base{bb} {}

  void group(std::uint16_t value) {
    // ctx.ctree.current()->get_info().order.group = value;
  };

  void up() {
    // kernel.override(kernel.get_selected(),z_order_t{});
  };

  void set(std::uint16_t value) {
    // ctx.ctree.current()->get_info().order.priority += value;
  };
};

struct builder_policy_interface : protected virtual builder_base {
  builder_policy_interface(builder_base &&bb) : builder_base{bb} {};

  void hovered(policy::hovered p) {
    kernel.override(kernel.get_selected(), p);
  };

  void event(policy::event p) { kernel.override(kernel.get_selected(), p); };
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

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  uid_t make(policy::shared sh, const std::string &str,
             const uid::anchor &anchor = default_anchor) {
    // ok

    auto el = kernel.get_selected();

    for (std::size_t i{0}; i < sh.up; ++i) {
      el = kernel.get_parent(el);
    }

    std::stringstream ss;
    ss << str;
    ss << &default_anchor;
    ss << el.self;

    auto hash_string = ss.str();

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  uid_t make(policy::unique, const std::string &str,
             const uid::anchor &anchor = default_anchor) {
    auto el = kernel.get_selected();

    std::stringstream ss;
    ss << str;
    ss << &default_anchor;
    ss << el.self;

    auto ch = kernel.get_childs(el, true);

    if (ch->valid()) {
      ss << kernel.get_uid(el).value();
    } else {
      ss << ch->get()->self;
      ss << kernel.get_uid(*ch->get()).value();
    }

    auto hash_string = ss.str();

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  uid_t make(policy::indexed, const std::string &str,
             const uid::anchor &anchor = default_anchor) {
    std::stringstream ss;
    ss << str;
    ss << &default_anchor;

    ss << kernel.get_selected().parent;
    ss << ++unit.top().index; // save | always contains root

    auto hash_string = ss.str();

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  uid_t self() const noexcept {
    return kernel.get_uid(kernel.get_selected()).value_or(0);
  };
};

struct not_function {};

template <typename R, typename... ARGS>
consteval R function_return_type_identyty(std::function<R(ARGS...)>);

template <typename T> consteval auto function_return_type(T &&obj) {
  if constexpr (requires() { std::function{obj}; }) {
    return std::type_identity<std::remove_cvref_t<
        decltype(function_return_type_identyty(std::function{obj}))>>{};
  } else {
    return std::type_identity<not_function>{};
  }
}

template <typename T>
using function_return_type_t =
    typename decltype(function_return_type(std::declval<T>()))::type;

template <typename T> struct ctor_info {
  using type = function_return_type_t<T>;

  static constexpr auto mtype() {
    return kernel::memory_model::type::from<type>();
  }
  // other create meta info
};

template <typename T>
concept ctor_cpt = not std::same_as<typename ctor_info<T>::type, not_function>;

struct builder_memory_interface : protected virtual builder_base {
  builder_memory_interface(builder_base &&bb) : builder_base{bb} {};

  template <typename T>
  void try_visit(iuic::uid_t uid, std::invocable<T &> auto &&call) {
    //
    auto *mem = kernel.memory();
    auto *type = kernel::memory_model::type::from<T>();

    if (mem->state(uid, type) ==
        kernel::memory_model::object_state::alive_this_type) {
      call(static_cast<T &>(mem->locate(uid, type)));
    }
  };

  void init_if_not(iuic::uid_t uid, ctor_cpt auto &&call) {
    using ctor_info = ctor_info<decltype(call)>;
    using T = ctor_info::type;
    auto *type = ctor_info::mtype();
    auto *mem = kernel.memory();

    if (auto state = mem->state(uid, type);
        state == kernel::memory_model::object_state::alive_this_type) {
      mem->update_livetime(uid);
    } else if (state == kernel::memory_model::object_state::reserve_this_type ||
               state == kernel::memory_model::object_state::reserve_none_type) {

      auto *located = static_cast<T *>(kernel.memory()->locate(uid, type));

      new (located) T{call()}; // call typed persist

      mem->launch(uid, type);
    }
  };

  template <typename T>
  void persist(iuic::uid_t uid, std::type_identity<T> = {}) {

    auto *mem = kernel.memory();
    auto *type = kernel::memory_model::type::from<T>();

    if (mem->state(uid) == kernel::memory_model::object_state::none_exist) {
      mem->reserve(uid, type);
    };
  };

  void persist(iuic::uid_t uid) {

    auto *mem = kernel.memory();

    if (mem->state(uid) == kernel::memory_model::object_state::none_exist) {
      mem->reserve(uid);
    };
  };
};

struct builder_event_interface : protected virtual builder_base {
  builder_event_interface(builder_base &&bb) : builder_base{bb} {};

  void operator()(event_callback_cpt auto &&call, uid_t object = 0) {
    attach(std::forward<decltype(call)>(call), object);
  };

  void operator()(custom_event_callback_cpt auto &&call,
                  iuic::uid_t object = 0) {
    attach(std::forward<decltype(call)>(call), object);
  };

  template <event_callback_cpt Call>
  void attach(Call &&call, iuic::uid_t object = 0) {
    collector.push(
        revent{std::forward<Call>(call), object, kernel.get_selected()});
  };

  void attach(custom_event_callback_cpt auto &&call, iuic::uid_t uid) {};
};

struct builder_style_interface : protected virtual builder_base {
  builder_style_interface(builder_base &&bb) : builder_base{bb} {};

  void override(style::shape &&shape) {
    // allocate tmp
    auto ptr = frame_memory<style::shape>();

    auto el = kernel.get_selected();
    new (ptr) style::shape{std::move(shape)};
    kernel.override(el, ptr);
  };

  void override(style::transform &&transform) {
    auto ptr = frame_memory<style::transform>();

    auto el = kernel.get_selected();

    new (ptr) style::transform{std::move(transform)};

    kernel.override(el, ptr);
  };

  void override(style::decoration &&decoration) {
    auto ptr = frame_memory<style::decoration>();

    auto el = kernel.get_selected();

    new (ptr) style::decoration{std::move(decoration)};

    kernel.override(el, ptr);
  };

  void override(std::invocable<style::transform &> auto &&call) {
    auto ptr = frame_memory<style::transform>();

    auto el = kernel.get_selected();

    new (ptr) style::transform{kernel.get_style(el).value()->get_transphorm()};

    kernel.override(el, ptr);

    call(*ptr);
  };

  void override(std::invocable<style::shape &> auto &&call) {
    auto ptr = frame_memory<style::shape>();

    auto el = kernel.get_selected();

    new (ptr) style::shape{kernel.get_style(el).value()->get_shape()};

    kernel.override(el, ptr);

    call(*ptr);
  };

  void override(std::invocable<style::decoration &> auto &&call) {
    auto ptr = frame_memory<style::decoration>();

    auto el = kernel.get_selected();

    new (ptr) style::decoration{kernel.get_style(el).value()->get_decoration()};

    kernel.override(el, ptr);

    call(*ptr);
  };

private:
  template <typename T> T *frame_memory() {
    return static_cast<T *>(
        kernel.memory()->tmp(kernel::memory_model::type::from<T>(), 1));
  };
};

struct builder_state_interface : protected virtual builder_base {
  builder_state_interface(builder_base &&bb) : builder_base{bb} {};

  bool hovered(iuic::uid_t uid) {
    return kernel.state()->has(uid, state::base::hovered());
  };

  bool has(iuic::uid_t uid, state s) { return kernel.state()->has(uid, s); }

  void attach(iuic::uid_t uid, state s) {
    return kernel.state()->attach(uid, s);
  };

  void detach(iuic::uid_t uid, state s) {
    return kernel.state()->detach(uid, s);
  };
};

struct builder final : public virtual builder_base,
                       private builder_element_interface,
                       private builder_state_interface,
                       private builder_uid_interface,
                       private builder_policy_interface,
                       private builder_memory_interface,
                       private builder_event_interface,
                       private builder_style_interface {
  friend void iuic::advance(auto &);
  // TOTO пересмотреть концепцию конструктора
  // перестроить его через kernel(module private)
  builder(builder_base &&bb) noexcept
      : builder_base{bb}, builder_element_interface{std::move(bb)},
        builder_state_interface{std::move(bb)},
        builder_uid_interface{std::move(bb)},
        builder_policy_interface{std::move(bb)},
        builder_memory_interface{std::move(bb)},
        builder_event_interface{std::move(bb)},
        builder_style_interface{std::move(bb)} {};
  builder_element_interface &element{*this};
  builder_state_interface &state{*this};
  builder_uid_interface &uid{*this};
  builder_policy_interface &policy{*this};
  builder_memory_interface &memory{*this};
  builder_event_interface &event{*this};
  builder_style_interface &style{*this};

public: // public forward decl
  builder(const builder &) = delete;
  builder &operator=(const builder &) = delete;
  builder(builder &&) = delete;
  builder &operator=(builder &&) = delete;
};

// impl

void builder_element_interface::frame(builder_block_cpt auto &&call,
                                      style::ref style,
                                      const frame_layout &layout) noexcept {

  auto el = kernel.instance(unit.top().uid, &layout, style);
  unit.push(unit_t{.uid = unit.top().uid});
  call(builder);
  kernel.launch(el);
};

void builder_element_interface::frame(builder_block_cpt auto &&call,
                                      const frame_layout &layout) noexcept {
  frame(std::forward<decltype(call)>(call), def_style, layout);
};

void builder_element_interface::frame(style::ref style,
                                      const frame_layout &layout) noexcept {
  kernel.launch(kernel.instance(unit.top().uid, &layout, style));
};

void builder_element_interface::frame(const frame_layout &layout) noexcept {
  frame(def_style, layout);
};

void builder_element_interface::frame(uid_t uid, builder_block_cpt auto &&call,
                                      style::ref style,
                                      const frame_layout &layout) noexcept {
  auto el = kernel.instance(uid, &layout, style);
  unit.push(unit_t{.uid = uid});
  call(builder);
  kernel.launch(el);
};

void builder_element_interface::frame(uid_t uid, builder_block_cpt auto &&call,
                                      const frame_layout &layout) noexcept {
  frame(uid, std::forward<decltype(call)>(call), def_style, layout);
}

void builder_element_interface::frame(uid_t uid, style::ref style,
                                      const frame_layout &layout) noexcept {
  kernel.launch(kernel.instance(uid, &layout, style));
};

void builder_element_interface::frame(uid_t uid,
                                      const frame_layout &layout) noexcept {
  frame(uid, def_style, layout);
};

/*
  Является конечной точкой.Отрисовка текста
*/
void builder_element_interface::text(iuic::text::token &&, style::ref style,
                                     const text_layout &layout) {
  // reg TPA
  // BROKEN
  kernel.launch(kernel.instance(unit.top().uid, &layout, style));
};

void builder_element_interface::text(const iuic::text::token &,
                                     style::ref style,
                                     const text_layout &layout) {
  // reg TPA
  // BROKEN
  kernel.launch(kernel.instance(unit.top().uid, &layout, style));
};
}; // namespace iuic::scheme
