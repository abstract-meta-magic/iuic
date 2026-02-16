// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.builder;
import std;
import iuic.underlying;
import iuic.state;
import :text.token;
import :text.buff;
import :text.present;
import :event;
import :policy;
import :machine.dispatcher;
import :environment.persist;
import :environment.tmp;

// forward
namespace iuic::layout {
struct frame;
struct text;
}; // namespace iuic::layout

namespace iuic::scheme {

struct sketch {
  struct element {
    std::variant<const layout::frame *, const layout::text *> layout;
    units::uid uid;
    style::sid sid;
    units::ui::zorder zorder;
  };
  environment::tmp env;
  utils::tree::flat_bfs_type<element> tree;
};

}; // namespace iuic::scheme

export namespace iuic::scheme {

struct builder;

template <typename T>
concept builder_block_cpt = std::invocable<T, builder &>;

struct builder_base {
protected:
  using insert_iterator =
      utils::tree::insert_iterator<utils::tree::node_type<sketch::element>>;
  using root_iterator =
      utils::tree::root_iterator<utils::tree::node_type<sketch::element>>;
  using sibling_iterator =
      utils::tree::root_iterator<utils::tree::node_type<sketch::element>>;

public:
  struct unit_t {
    units::uid uid{0};
    std::size_t index{0};
  };

  builder_base(environment::tmp &tenv_, environment::persist &penv_,
               insert_iterator it_)
      : tenv{tenv_}, penv{penv_}, it{it_} {

          //   static std::string root_uid{"root-uid-hash-str-4467532667"};
          // unit.push(unit_t{.uid =
          // kernel.hash(std::as_bytes(std::span(root_uid)))});
        };

protected: // builder unit stack
  environment::tmp &tenv;
  environment::persist &penv;
  insert_iterator it;
};

struct builder_element_interface : protected virtual builder_base {
  builder_element_interface(builder_base &&bb) : builder_base{bb} {};

  /*
    Базовая форма для всего.Стелизуемый рамка.
    TODO : можно заменить на нешаблонный вызов
  */
  void frame(const style::decl &, const layout::frame &,
             builder_block_cpt auto &&call) noexcept;

  void frame(units::uid uid, const style::decl &, const layout::frame &,
             builder_block_cpt auto &&call) noexcept;

  /*
    Является конечной точкой.Отрисовка текста
  */
  void text(iuic::text::token &&, const style::decl *, const layout::text &);

  // in frame
  void text(const iuic::text::token &, const style::decl *,
            const layout::text &);
};

struct builder_order_interface : protected virtual builder_base {
  builder_order_interface(builder_base &&bb) : builder_base{bb} {}

  void group(std::uint16_t value);

  void up();

  void set(std::uint16_t value);
};

struct builder_policy_interface : protected virtual builder_base {
  builder_policy_interface(builder_base &&bb) : builder_base{bb} {};

  void hovered(policy::hovered p);

  void event(policy::event p);
};

struct builder_uid_interface : protected virtual builder_base {
  static inline utils::anchor default_anchor{};

  builder_uid_interface(builder_base &&bb) : builder_base{bb} {};

  units::uid make(const std::string &str,
                  const utils::anchor &anchor = default_anchor) const noexcept;

  units::uid make(policy::shared sh, const std::string &str,
                  const utils::anchor &anchor = default_anchor);

  units::uid make(policy::unique, const std::string &str,
                  const utils::anchor &anchor = default_anchor);

  units::uid make(policy::indexed, const std::string &str,
                  const utils::anchor &anchor = default_anchor);

  units::uid self() const noexcept;
};

struct builder_memory_interface : protected virtual builder_base {
  builder_memory_interface(builder_base &&bb) : builder_base{bb} {};

  template <typename T>
  void try_visit(units::uid uid, std::invocable<T &> auto &&call);

  // void init_if_not(units::uid uid, ctor_cpt auto &&call);

  template <typename T> void typed_dirty(units::uid uid);

  void dirty(units::uid uid);

  template <typename T>
  void persist(units::uid uid, std::type_identity<T> = {});
};

struct builder_event_interface : protected virtual builder_base {
  builder_event_interface(builder_base &&bb) : builder_base{bb} {};

  void operator()(event::callback_cpt auto &&call, units::uid object = 0);

  template <event::callback_cpt Call>
  void attach(Call &&call, units::uid object = 0);
};

struct builder_style_interface : protected virtual builder_base {
  builder_style_interface(builder_base &&bb) : builder_base{bb} {};

  void override(style::shape &&shape);

  void override(style::transform &&transform);

  void override(style::decoration &&decoration);

  void override(std::invocable<style::transform &> auto &&call);

  void override(std::invocable<style::shape &> auto &&call);

  void override(std::invocable<style::decoration &> auto &&call);

private:
  template <typename T> T *frame_memory();
};

struct builder_state_interface : protected virtual builder_base {
  builder_state_interface(builder_base &&bb) : builder_base{bb} {};

  bool has(units::uid uid, state::value s);
  void attach(units::uid uid, state::value s);

  void detach(units::uid uid, state::value s);

  struct machine_accessor {
    machine_accessor(builder_state_interface &i_) : i{i_} {};

    void use(const auto &proto);

    void use(units::uid uid, const auto &proto);

    void transition(state::value state);

    void transition(units::uid uid, state::value state);

    // TOTO : error handling
    // void transition(units::uid, iuic::state, auto err);
    // void transition(iuic::state, auto err);

    void try_visit_shared(
        erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call);

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

}; // namespace iuic::scheme

namespace iuic::scheme {

struct director {
  director(environment::persist &);

  sketch make(std::invocable<builder &> auto &&call) {
    environment::tmp tenv;
    utils::tree::node_type<sketch::element> tree;

    // builder b{builder_base{penv, tenv, tree.insert_point()}};

    // call(b);

    // make blueprint
    // etc
  };

private:
  environment::persist &penv;
};
}; // namespace iuic::scheme
