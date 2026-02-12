// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.builder;
import std;
import iuic.underlying;
import iuic.state;
import :kernel;
import :text.token;
import :text.buff;
import :text.present;
import :layout.frame.box;
import :layout.text.box;
import :event;
import :scheme;
import :machine.dispatcher;
import :environment.persist;
import :environment.tmp;

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
    units::uid uid{0};
    std::size_t index{0};
  };

  builder_base(kernel::root &ctx_, event::collector &collector_,
               iuic::text::present::aggregator &tpa_,
               state::machine::dispatcher &md_, builder &builder_)
      : kernel{ctx_}, collector{collector_}, tpa{tpa_}, machine_dispatcher{md_},
        builder{builder_} {
    static std::string root_uid{"root-uid-hash-str-4467532667"};
    unit.push(unit_t{.uid = kernel.hash(std::as_bytes(std::span(root_uid)))});
  };

protected: // builder unit stack
  kernel::root &kernel;
  event::collector &collector;
  iuic::text::present::aggregator &tpa;
  state::machine::dispatcher &machine_dispatcher;
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

  void frame(units::uid uid, builder_block_cpt auto &&call,
             style::ref = def_style,
             const frame_layout & = box_layout) noexcept;

  void frame(units::uid uid, builder_block_cpt auto &&call,
             const frame_layout &) noexcept;

  void frame(units::uid uid, style::ref = def_style,
             const frame_layout & = box_layout) noexcept;

  void frame(units::uid uid, const frame_layout &) noexcept;

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
  static inline utils::anchor default_anchor{};

  builder_uid_interface(builder_base &&bb) : builder_base{bb} {};

  units::uid make(const std::string &str,
                  const utils::anchor &anchor = default_anchor) const noexcept {
    std::stringstream ss;
    ss << str;
    ss << anchor.value;

    auto hash_string = ss.str();

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  units::uid make(policy::shared sh, const std::string &str,
                  const utils::anchor &anchor = default_anchor) {
    // ok

    auto el = kernel.get_selected();

    for (std::size_t i{0}; i < sh.up; ++i) {
      el = kernel.get_parent(el);
    }

    std::stringstream ss;
    ss << str;
    ss << anchor.value;
    ss << el.self;

    auto hash_string = ss.str();

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  units::uid make(policy::unique, const std::string &str,
                  const utils::anchor &anchor = default_anchor) {
    auto el = kernel.get_selected();

    std::stringstream ss;
    ss << str;
    ss << anchor.value;
    ss << el.self;

    auto ch = kernel.get_childs(el);

    if (not ch->valid()) {
      ss << kernel.get_uid(el).value();
    } else {
      kernel::element che;
      for (auto &e : ch->range()) {
        che = e;
      }
      ss << ch->get()->self;
      ss << kernel.get_uid(che).value();
    }

    auto hash_string = ss.str();
    auto uid = kernel.hash(std::as_bytes(std::span(hash_string)));

    return uid;
  };

  units::uid make(policy::indexed, const std::string &str,
                  const utils::anchor &anchor = default_anchor) {
    std::stringstream ss;
    ss << str;
    ss << anchor.value;

    ss << kernel.get_selected().parent;
    ss << ++unit.top().index; // save | always contains root

    auto hash_string = ss.str();

    return kernel.hash(std::as_bytes(std::span(hash_string)));
  };

  units::uid self() const noexcept {
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

  static constexpr auto mtype() { return erasure::type::from<type>(); }
  // other create meta info
};

template <typename T>
concept ctor_cpt = not std::same_as<typename ctor_info<T>::type, not_function>;

struct builder_memory_interface : protected virtual builder_base {
  builder_memory_interface(builder_base &&bb) : builder_base{bb} {};

  template <typename T>
  void try_visit(units::uid uid, std::invocable<T &> auto &&call) {
    //
    auto *mem = kernel.memory();
    auto *type = erasure::type::from<erasure::pure_t<T>>();

    if (mem->state(uid, type) ==
        kernel::memory_model::object_state::alive_this_type) {
      if (auto *locate = mem->locate(uid, type)) {
        call(*static_cast<T *>(locate));
      }
    }
  };

  void init_if_not(units::uid uid, ctor_cpt auto &&call) {
    using ctor_info = ctor_info<decltype(call)>;
    using T = ctor_info::type;
    auto *type = ctor_info::mtype();
    auto *mem = kernel.memory();

    if (auto state = mem->state(uid, type);
        state == kernel::memory_model::object_state::outdated_this_type) {
      mem->update_livetime(uid);
    } else if (state == kernel::memory_model::object_state::reserve_this_type ||
               state == kernel::memory_model::object_state::reserve_none_type) {
      mem->launch(uid, type);

      auto *located = static_cast<T *>(kernel.memory()->locate(uid, type));

      new (located) T{call()}; // call typed persist
    }
  };

  template <typename T> void typed_dirty(units::uid uid) {
    auto *mem = kernel.memory();
    mem->reserve(uid, erasure::type::from<T>());
  };

  void dirty(units::uid uid) {
    auto *mem = kernel.memory();
    mem->reserve(uid);
  };

  template <typename T>
  void persist(units::uid uid, std::type_identity<T> = {}) {

    auto *mem = kernel.memory();
    auto *type = erasure::type::from<T>();

    if (mem->state(uid) == kernel::memory_model::object_state::none_exist) {
      mem->reserve(uid, type);
    };
  };

  void persist(units::uid uid) {

    auto *mem = kernel.memory();

    if (mem->state(uid) == kernel::memory_model::object_state::none_exist) {
      mem->reserve(uid);
    };
  };
};

struct builder_event_interface : protected virtual builder_base {
  builder_event_interface(builder_base &&bb) : builder_base{bb} {};

  void operator()(event::callback_cpt auto &&call, units::uid object = 0) {
    attach(std::forward<decltype(call)>(call), object);
  };

  template <event::callback_cpt Call>
  void attach(Call &&call, units::uid object = 0) {
    collector.push(
        event::row{std::forward<Call>(call), object, kernel.get_selected()});
  };
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
    return static_cast<T *>(kernel.memory()->tmp(erasure::type::from<T>(), 1));
  };
};

struct builder_state_interface : protected virtual builder_base {
  builder_state_interface(builder_base &&bb) : builder_base{bb} {};

  bool hovered(units::uid uid) {
    return kernel.state()->has(uid, state::base::hovered);
  };

  bool has(units::uid uid, state::value s) {
    return kernel.state()->has(uid, s);
  }

  void attach(units::uid uid, state::value s) {
    if (kernel.state()->is_exist(uid)) {
      kernel.state()->update_livetime(uid);
    } else {
      kernel.state()->attach(uid, s);
    }
  };

  void detach(units::uid uid, state::value s) {
    return kernel.state()->detach(uid, s);
  };

  void transfer(state::value from, state::value to, auto *coro) {
    //
  };

  struct machine_accessor {
    machine_accessor(builder_state_interface &i_) : i{i_} {};

    void use(const auto &proto) {
      auto el = i.kernel.get_selected();
      i.machine_dispatcher.machine_instance(i.kernel.get_uid(el).value(),
                                            proto);
    };

    void use(units::uid uid, const auto &proto) {
      i.machine_dispatcher.machine_instance(uid, proto);
    };

    void transition(state::value state) {
      std::println("eua");
      auto el = i.kernel.get_selected();
      auto *m = i.machine_dispatcher.get_machine(i.kernel.get_uid(el).value());

      if (m) {
        std::println("eua");
        m->get_controller().try_move(state);
      }
    };

    void transition(units::uid uid, state::value state) {
      auto *m = i.machine_dispatcher.get_machine(uid);

      if (m) {
        m->get_controller().try_move(state);
      }
    };

    // TOTO : error handling
    // void transition(units::uid, iuic::state, auto err);
    // void transition(iuic::state, auto err);

    void try_visit_shared(
        erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call) {
      auto el = i.kernel.get_selected();
      auto *m = i.machine_dispatcher.get_machine(i.kernel.get_uid(el).value());

      if (m) {
        m->get_controller().try_visit_shared(
            std::forward<decltype(call)>(call));
      }
    };

  private:
    builder_state_interface &i;
  } machine{*this};
};

struct builder final : public virtual builder_base,
                       private builder_element_interface,
                       private builder_state_interface,
                       private builder_uid_interface,
                       private builder_policy_interface,
                       private builder_memory_interface,
                       private builder_event_interface,
                       private builder_style_interface {
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
  auto el = kernel.instance(unit.top().uid, &layout, style);
  kernel.launch(el);
};

void builder_element_interface::frame(const frame_layout &layout) noexcept {
  frame(def_style, layout);
};

void builder_element_interface::frame(units::uid uid,
                                      builder_block_cpt auto &&call,
                                      style::ref style,
                                      const frame_layout &layout) noexcept {
  kernel.state()->update_livetime(uid);
  auto el = kernel.instance(uid, &layout, style);
  unit.push(unit_t{.uid = uid});
  call(builder);
  kernel.launch(el);
};

void builder_element_interface::frame(units::uid uid,
                                      builder_block_cpt auto &&call,
                                      const frame_layout &layout) noexcept {
  kernel.state()->update_livetime(uid);
  frame(uid, std::forward<decltype(call)>(call), def_style, layout);
}

void builder_element_interface::frame(units::uid uid, style::ref style,
                                      const frame_layout &layout) noexcept {
  kernel.state()->update_livetime(uid);
  auto el = kernel.instance(uid, &layout, style);
  kernel.launch(el);
};

void builder_element_interface::frame(units::uid uid,
                                      const frame_layout &layout) noexcept {
  kernel.state()->update_livetime(uid);
  frame(uid, def_style, layout);
};

/*
  Является конечной точкой.Отрисовка текста
*/
void builder_element_interface::text(iuic::text::token &&token,
                                     style::ref style,
                                     const text_layout &layout) {
  // reg TPA
  // BROKEN
  auto el = kernel.instance(unit.top().uid, &layout, style);

  auto mem = kernel.memory()->tmp(erasure::type::from<iuic::text::token>());

  new (mem) iuic::text::token{std::move(token)};

  tpa.reserve_present(el.self, iuic::text::token::sequence{
                                   static_cast<iuic::text::token *>(mem), 1});
  kernel.launch(el);
};

void builder_element_interface::text(const iuic::text::token &token,
                                     style::ref style,
                                     const text_layout &layout) {
  // reg TPA
  // BROKEN
  auto el = kernel.instance(unit.top().uid, &layout, style);

  auto mem = kernel.memory()->tmp(erasure::type::from<iuic::text::token>());

  new (mem) iuic::text::token{token};

  tpa.reserve_present(el.self, iuic::text::token::sequence{
                                   static_cast<iuic::text::token *>(mem), 1});
  kernel.launch(el);
};
}; // namespace iuic::scheme

namespace iuic::scheme {
struct blueprint {
  struct element {
    units::uid uid;
    std::variant<const frame_layout *, const text_layout *> layout;
  };

  environment::tmp env;
  utils::tree::flat_bfs_type<el> tree;
};

struct director {
  director(environment::persist &);

  blueprint make(std::invocable<builder &> auto &&call) {
    environment::tmp tenv;
    utils::tree::node_type<blueprint::element> tree;
    // builder b{...};

    // b.call(std::forward<decltype(call)>(call));

    // make blueprint
    // etc
  };

private:
  environment::persist &env;
};
}; // namespace iuic::scheme
