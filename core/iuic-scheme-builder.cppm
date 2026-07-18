// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.builder;
import std;
import iuic.underlying;
import iuic.state;
import iuic.text;
import iuic.event;
import :policy;
import :scheme.base;
// import :proto.base;

export namespace iuic::scheme {

struct builder;

template <typename T>
concept builder_block_cpt = std::invocable<T, builder &>;

struct builder_base {
protected:
  using insert_iterator =
      tree::insert_iterator<tree::flat_unordered_type<sketch::value_t>>;

public:
  struct unit_t {
    units::uid uid{0};
    std::size_t index{0};
  };

  builder_base(environment::tmp &tenv_, environment::persist &penv_,
               insert_iterator it_, builder &builder_)
      : tenv{tenv_}, penv{penv_}, it{it_}, builder{builder_} {

    unique_uid.reserve(400);
  };

protected: // builder unit stack
  environment::tmp &tenv;
  environment::persist &penv;
  insert_iterator it;
  builder &builder;
  std::vector<std::size_t> unique_uid;
  std::uint32_t index{0};
};

struct builder_element_interface : protected virtual builder_base {

  // TODO :
  // Разделить 64 hash на 32(группа)|32(елемент)

  builder_element_interface(environment::tmp &tenv_,
                            environment::persist &penv_, insert_iterator it_,
                            struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  /*
    Базовая форма для всего.Стелизуемый рамка.
    TODO : можно заменить на нешаблонный вызов
  */
  void frame(style::sid, const layout::frame &,
             builder_block_cpt auto &&call) noexcept;

  void frame(units::uid uid, style::sid, const layout::frame &,
             builder_block_cpt auto &&call) noexcept;

  void frame(units::uid uid, style::sid, const layout::frame &) noexcept;

  void frame(style::sid, const layout::frame &) noexcept;

  /*
    Является конечной точкой.Отрисовка текста
  */
  void text(const iuic::text::raw::token &token, iuic::style::sid sid,
            const layout::text &);

  void text(std::span<const iuic::text::raw::token> tokens,
            iuic::style::sid sid, const layout::text &);
};

struct builder_order_interface : protected virtual builder_base {
  builder_order_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  void group(std::uint16_t value);

  void up();

  void set(std::uint16_t value);
};

struct builder_policy_interface : protected virtual builder_base {
  builder_policy_interface(environment::tmp &tenv_, environment::persist &penv_,
                           insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  void set(auto p);
};

struct builder_uid_interface : protected virtual builder_base {
  static inline utils::anchor default_anchor{};

  builder_uid_interface(environment::tmp &tenv_, environment::persist &penv_,
                        insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

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
  builder_memory_interface(environment::tmp &tenv_, environment::persist &penv_,
                           insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  // TODO : MB replace to decoy
  template <typename T>
  bool try_visit(units::uid uid, std::invocable<T &> auto &&call);

  void init_if_not(units::uid uid, std::invocable<> auto &&call);

  template <typename T>
  void persist(units::uid uid, std::type_identity<T> = {});

  template <typename T> T &tmp(T &&);
};

struct builder_event_interface : protected virtual builder_base {
  builder_event_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  template <const iuic::event::channel &ch> void emit(auto &&data) {
    tenv.event.emit<ch>(data);
  };

  // custom
};

struct builder_text_interface : protected virtual builder_base {
  builder_text_interface(environment::tmp &tenv_, environment::persist &penv_,
                         insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  // UTF-8
  const text::raw::token &static_token(text::atlas::id, std::string_view);

  // UTF-8
  const text::raw::token &dynamic_token(text::atlas::id, std::string_view);
};

struct builder_style_interface : protected virtual builder_base {
  builder_style_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  style::sid self();

  style::sid override(style::sid sid);

  style::sid make(style::sid sid);

  style::sid make(const style::decl *s);

  style::sid make(const style::decl &s);

  style::sid make(style::sid sid, style::shape &&);

  style::sid make(style::sid sid, style::transform &&);

  style::sid make(style::sid sid, style::decoration &&);
};

struct builder_state_interface : protected virtual builder_base {
  builder_state_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{tenv_, penv_, it_, builder_} {}

  bool has(units::uid uid, state::value s);

  void attach(units::uid uid, state::value s);

  void detach(units::uid uid, state::value s);

  void persist(units::uid uid);

  struct : utils::member_for<builder_state_interface> {
    void use(const auto &proto);

    void use(units::uid uid, const auto &proto);

    void transition(state::value state);

    void transition(units::uid uid, state::value state);
    // TOTO : error handling
    // void transition(units::uid, iuic::state, auto err);
    // void transition(iuic::state, auto err);

    void try_visit_shared(
        erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call);

  } machine{*this};
};

struct builder final : public virtual builder_base,
                       private builder_element_interface,
                       private builder_state_interface,
                       private builder_uid_interface,
                       private builder_policy_interface,
                       private builder_memory_interface,
                       private builder_text_interface,
                       private builder_event_interface,
                       private builder_style_interface {
  // TOTO пересмотреть концепцию конструктора
  // перестроить его через kernel(module private)
  builder(environment::tmp &tenv, environment::persist &penv,
          insert_iterator it) noexcept
      : builder_base{tenv, penv, it, *this},
        builder_element_interface{tenv, penv, it, *this},
        builder_state_interface{tenv, penv, it, *this},
        builder_uid_interface{tenv, penv, it, *this},
        builder_policy_interface{tenv, penv, it, *this},
        builder_memory_interface{tenv, penv, it, *this},
        builder_text_interface{tenv, penv, it, *this},
        builder_event_interface{tenv, penv, it, *this},
        builder_style_interface{tenv, penv, it, *this} {};

  builder_element_interface &element{*this};
  builder_state_interface &state{*this};
  builder_uid_interface &uid{*this};
  builder_policy_interface &policy{*this};
  builder_memory_interface &memory{*this};
  builder_event_interface &event{*this};
  builder_style_interface &style{*this};
  builder_text_interface &text{*this};

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
  director(environment::tmp &tenv_, environment::persist &penv_)
      : tenv{tenv_}, penv{penv_} {};

  sketch make(units::ui::size viewport, std::invocable<builder &> auto &&call) {
    tenv.meta.viewport_size = viewport;
    tree::flat_unordered_type<sketch::value_t> tree;

    builder b{tenv, penv, {tree.root()}};

    call(b);

    return sketch{tree::move_iterator{tree.root()}};
  };

private:
  environment::tmp &tenv;
  environment::persist &penv;
};

// ---- IMPL ----

// ---- IMPL [elemnet] ----
void builder_element_interface::frame(style::sid sid_,
                                      const layout::frame &layout,
                                      builder_block_cpt auto &&call) noexcept {
  auto ait = tree::access_iterator{it};

  auto nit = it.at(
      sketch::value_t{.layout = &layout,
                      .uid = ait ? ait->uid : units::uid{0},
                      .sid = sid_,
                      .order = ait ? units::ui::order{index++, ait->order.layer}
                                   : units::ui::order{index++, 0}});

  std::swap(nit, it);
  call(builder);
  std::swap(nit, it);
};

void builder_element_interface::frame(units::uid uid_, style::sid sid_,
                                      const layout::frame &layout_,
                                      builder_block_cpt auto &&call) noexcept {
  auto ait = tree::access_iterator{it};

  auto nit = it.at(
      sketch::value_t{.layout = &layout_,
                      .uid = uid_,
                      .sid = sid_,
                      .order = ait ? units::ui::order{index++, ait->order.layer}
                                   : units::ui::order{index++, 0}});

  std::swap(nit, it);
  call(builder);
  std::swap(nit, it);
};
void builder_element_interface::frame(units::uid uid_, style::sid sid_,
                                      const layout::frame &layout_) noexcept {
  auto ait = tree::access_iterator{it};

  auto nit = it.at(
      sketch::value_t{.layout = &layout_,
                      .uid = uid_,
                      .sid = sid_,
                      .order = ait ? units::ui::order{index++, ait->order.layer}
                                   : units::ui::order{index++, 0}});
};

void builder_element_interface::frame(style::sid sid_,
                                      const layout::frame &layout_) noexcept {

  auto ait = tree::access_iterator{it};

  auto nit = it.at(
      sketch::value_t{.layout = &layout_,
                      .uid = ait ? ait->uid : units::uid{0},
                      .sid = sid_,
                      .order = ait ? units::ui::order{index++, ait->order.layer}
                                   : units::ui::order{index++, 0}});
};

void builder_element_interface::text(const iuic::text::raw::token &token,
                                     iuic::style::sid sid_,
                                     const layout::text &layout_) {
  auto ait = tree::access_iterator{it};

  auto nit = it.at(sketch::value_t{
      .layout = &layout_,
      .uid = ait ? ait->uid : units::uid{0},
      .sid = sid_,
      .order = ait ? units::ui::order{index++, ait->order.layer}
                   : units::ui::order{index++, 0},
      .text = {&token, 1} // SINGLE TOKEN SPAN
  });
};

void builder_element_interface::text(
    std::span<const iuic::text::raw::token> tokens, iuic::style::sid sid_,
    const layout::text &layout_) {
  auto ait = tree::access_iterator{it};

  auto nit = it.at(
      sketch::value_t{.layout = &layout_,
                      .uid = ait ? ait->uid : units::uid{0},
                      .sid = iuic::style::sid{0},
                      .order = ait ? units::ui::order{index++, ait->order.layer}
                                   : units::ui::order{index++, 0},
                      .text = tokens

      });
};
// ---- IMPL [uid] ----

units::uid
builder_uid_interface::make(const std::string &str,
                            const utils::anchor &anchor) const noexcept {

  std::stringstream ss;
  ss << anchor.value;
  ss << str;
  return units::uid{
      static_cast<std::uint64_t>(std::hash<std::string>{}(ss.str()))};
};

units::uid builder_uid_interface::make(policy::shared sh,
                                       const std::string &str,
                                       const utils::anchor &anchor) {
  std::stringstream ss;
  ss << anchor.value;
  ss << "shared--";
  ss << str;
  tree::root_iterator rit{it};

  if (++rit) {
    ss << std::to_underlying(tree::access_iterator{++rit}->uid);
  } else {
    ss << "--root-of";
  }

  return units::uid{
      static_cast<std::uint64_t>(std::hash<std::string>{}(ss.str()))};
};

units::uid builder_uid_interface::make(policy::unique, const std::string &str,
                                       const utils::anchor &anchor) {
  static thread_local char buff[512]{"--unique"};

  std::size_t size{8};

  if (unique_uid.empty()) {
    static char first[]{"--first"};
    std::memcpy(&buff[9], first, 8);
    size += 8;
  } else {
    static char last[]{"--last--unique"};
    std::memcpy(&buff[9], last, 15);
    std::memcpy(&buff[24], &unique_uid.back(), sizeof(std::size_t));
    size += 8 + 15 + sizeof(std::size_t);
  }

  std::size_t hash = std::hash<std::string_view>{}({buff, size});

  unique_uid.push_back(hash);

  // std::println("hash : {}", hash);

  return units::uid{static_cast<std::uint64_t>(hash)};
};

units::uid builder_uid_interface::self() const noexcept {
  return tree::access_iterator{it}->uid;
};

// ---- IMPL [state] ----

void builder_state_interface::attach(units::uid uid, state::value v) {
  penv.state.attach(uid, v);
};
void builder_state_interface::detach(units::uid uid, state::value v) {
  penv.state.detach(uid, v);
};
bool builder_state_interface::has(units::uid uid, state::value v) {
  return penv.state.has(uid, v);
};

void builder_state_interface::persist(units::uid uid) {
  penv.state.update_lifetime(uid);
};

void utils::type_of<&builder_state_interface::machine>::use(const auto &proto) {
  // TODO : Impl unknown
}

void utils::type_of<&builder_state_interface::machine>::try_visit_shared(
    erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&visitor) {
  auto *machine =
      self().penv.machine.get(tree::access_iterator{self().it}->uid);
  if (machine) {
    machine->get_controller().try_visit_shared(
        std::forward<decltype(visitor)>(visitor));
  }
};

void utils::type_of<&builder_state_interface::machine>::transition(
    state::value v) {
  auto *machine =
      self().penv.machine.get(tree::access_iterator{self().it}->uid);
  if (machine) {
    // Mb no move ??
    machine->get_controller().try_move(v);
  }
};

// ---- IMPL [event] ----

// ---- IMPL [style] ----

style::sid builder_style_interface::self() {
  return tree::access_iterator{it}->sid;
};

style::sid builder_style_interface::override(style::sid sid) {
  return tree::access_iterator{it}->sid = sid;
};

style::sid builder_style_interface::make(style::sid sid) {
  return tenv.style.make(sid);
};

style::sid builder_style_interface::make(const style::decl *s) {
  return tenv.style.make(s);
};

style::sid builder_style_interface::make(const style::decl &s) {
  return tenv.style.make(s);
};

style::sid builder_style_interface::make(style::sid sid, style::shape &&shape) {
  return tenv.style.override(sid, std::move(shape));
};

style::sid builder_style_interface::make(style::sid sid,
                                         style::transform &&transform) {
  return tenv.style.override(sid, std::move(transform));
};

style::sid builder_style_interface::make(style::sid sid,
                                         style::decoration &&decoration) {
  return tenv.style.override(sid, std::move(decoration));
};

// ---- IMPL [memory] ----
template <typename T> T &builder_memory_interface::tmp(T &&value) {
  T *ptr = static_cast<T *>(tenv.memory.allocate(sizeof(T), alignof(T)));
  new (ptr) T{std::move(value)};
  return *ptr;
};

template <typename T>
void builder_memory_interface::persist(units::uid uid, std::type_identity<T>) {
  auto state = penv.object.state<T>(uid);

  if (state == penv.object.non_exist || state == penv.object.deleted) {
    penv.object.reserve<T>(uid);
  }
};

template <typename T>
bool builder_memory_interface::try_visit(units::uid uid,
                                         std::invocable<T &> auto &&call) {
  return penv.object.get(uid).try_visit(std::forward<decltype(call)>(call));
};

void builder_memory_interface::init_if_not(units::uid uid,
                                           std::invocable<> auto &&call) {
  using traits = typename decltype(erasure::func_type{call})::traits;

  auto state =
      penv.object.state<std::remove_cvref_t<typename traits::return_t>>(uid);

  if (state == penv.object.reserve_this_type ||
      state == penv.object.reserve_undefined_type) {
    penv.object.construct(uid, std::forward<decltype(call)>(call));
  } else if (state == penv.object.alive_this_type ||
             state == penv.object.outdated_this_type) {
    penv.object.update_lifetime(uid);
  }
};

// ---- IMPL [text] ----
const text::raw::token &
builder_text_interface::static_token(text::atlas::id id,
                                     std::string_view text) {
  struct data {
    text::raw::token token;
    std::vector<iuic::text::glyph::id_t> memory;
  };

  static text::raw::token inv{.atlas_id = text::atlas::invalid_id};
  static std::unordered_map<text::atlas::id,
                            std::unordered_map<std::string, data>>
      cache;
  auto &atlas = text::atlas::by_id(id);

  if (atlas.get_id() == text::atlas::invalid_id || text.size() > 50) {
    return inv;
  }

  if (atlas.decoder->capabilities().is_std_char_support()) {
    if (auto map = cache.find(id); map != cache.end()) {
      if (auto token = map->second.find(std::string{text});
          token != map->second.end()) {
        return token->second.token;
      }
    }
    // wrap to try-block ??
    auto res = atlas.decoder->decode(text); // exceptions

    text::raw::token tk{.atlas_id = id, .glyphs = res};

    auto it = cache[id].insert({std::string{text}, {}});

    if (it.second) {
      auto &d = it.first->second;
      d.memory = res;
      d.token.atlas_id = id;
      d.token.glyphs = d.memory;
    }

    return it.first->second.token;
  }

  return inv;
};

// UTF-8
const text::raw::token &
builder_text_interface::dynamic_token(text::atlas::id id,
                                      std::string_view text) {
  static text::raw::token inv{.atlas_id = text::atlas::invalid_id};

  auto &atlas = text::atlas::by_id(id);

  if (atlas.get_id() == text::atlas::invalid_id || text.size() > 50) {
    return inv;
  }

  if (atlas.decoder->capabilities().is_std_char_support()) {
    auto res = atlas.decoder->decode(text); // exceptions

    text::raw::token tk{.atlas_id = id, .glyphs = res};

    auto *tk_mem = static_cast<text::raw::token *>(tenv.memory.allocate(
        sizeof(text::raw::token), alignof(text::raw::token)));

    auto *gl_mem = static_cast<text::glyph::id_t *>(tenv.memory.allocate(
        sizeof(text::glyph::id_t), alignof(text::glyph::id_t), res.size()));

    if (gl_mem) {
      for (std::size_t i{0}; i < res.size(); ++i) {
        gl_mem[i] = res[i];
      }
    }

    tk_mem->atlas_id = id;
    tk_mem->glyphs = {gl_mem, res.size()};

    return *tk_mem;
  }

  return inv;
};
// ---- IMPL [policy] ----

void builder_policy_interface::set(auto p) {
  tenv.policy.set(tree::access_iterator{it}->uid, p);
};
}; // namespace iuic::scheme
