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

namespace iuic::scheme {

export struct builder;

template <typename T>
concept builder_block_cpt = std::invocable<T, builder &>;

struct builder_base {
  using insert_iterator =
      tree::insert_iterator<tree::flat_unordered_type<sketch::value_t>>;

public:
  struct unit_t {
    units::uid uid{0};
    std::size_t index{0};
  };

  builder_base(environment::persist &penv_, environment::tmp &tenv_,
               insert_iterator it_, builder &builder_)
      : penv{penv_}, tenv{tenv_}, it{it_}, builder{builder_} {
    deep_index.reserve(200);
    deep_index.push_back({});
  }

  environment::persist &penv;
  environment::tmp &tenv;
  insert_iterator it;
  builder &builder;

  struct index {
    std::size_t position{0};
    std::size_t branch{0};
  };

  std::vector<index> deep_index{};
  std::size_t current_deep{0};
  std::uint32_t global_index{0};
};

struct builder_element_interface : protected virtual builder_base {

  // TODO :
  // Разделить 64 hash на 32(группа)|32(елемент)

  builder_element_interface(environment::tmp &tenv_,
                            environment::persist &penv_, insert_iterator it_,
                            struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

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

  void text(const iuic::text::raw::token &token, units::uid uid,
            iuic::style::sid sid, const layout::text &);

  void text(std::span<const iuic::text::raw::token> tokens,
            iuic::style::sid sid, const layout::text &);

  void text(std::span<const iuic::text::raw::token> tokens, units::uid uid,
            iuic::style::sid sid, const layout::text &);
};

struct builder_order_interface : protected virtual builder_base {
  builder_order_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

  void group(std::uint16_t value);

  void up();

  void set(std::uint16_t value);
};

struct builder_policy_interface : protected virtual builder_base {
  builder_policy_interface(environment::tmp &tenv_, environment::persist &penv_,
                           insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

  void set(auto p);

  void set(units::uid uid, auto p);

  template <typename T> T get();

  template <typename T> T get(units::uid uid);
};

struct builder_uid_interface : protected virtual builder_base {
  static inline utils::anchor default_anchor{};

  builder_uid_interface(environment::tmp &tenv_, environment::persist &penv_,
                        insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

  units::uid
  make_static(const std::string &str,
              const utils::anchor &anchor = default_anchor) const noexcept;

  units::uid make_ordered(const std::string &str,
                          const utils::anchor &anchor = default_anchor);

  units::uid make_stable(const std::string &str, const utils::anchor &anchor);

  units::uid self() const noexcept;
};

struct builder_memory_interface : protected virtual builder_base {
  builder_memory_interface(environment::tmp &tenv_, environment::persist &penv_,
                           insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

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
      : builder_base{penv_, tenv_, it_, builder_} {}

  template <const iuic::event::channel &ch> void emit(auto &&...args) {
    if constexpr (requires { tenv.event.emit<ch>(builder, args...); }) {
      tenv.event.emit<ch>(builder, args...);
    } else {
      tenv.event.emit<ch>(args...);
    }
  };

  // custom
};

struct builder_text_interface : protected virtual builder_base {
  builder_text_interface(environment::tmp &tenv_, environment::persist &penv_,
                         insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

  // UTF-8
  const text::raw::token &static_token(std::string_view, std::string_view);

  // UTF-8
  const text::raw::token &dynamic_token(std::string_view, std::string_view);
};

struct builder_style_interface : protected virtual builder_base {
  builder_style_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

  style::sid self();

  style::sid make(const style::decl *s);

  style::sid make(const style::decl &s);

  // throw exception
  void override(style::sid sid, std::invocable<style::shape &> auto &&call);

  // throw exception
  void override(style::sid sid,
                std::invocable<style::decoration &> auto &&call);

  // throw exception
  void override(style::sid sid, std::invocable<style::transform &> auto &&call);

  bool try_override(style::sid sid, std::invocable<style::shape &> auto &&call);

  bool try_override(style::sid sid,
                    std::invocable<style::decoration &> auto &&call);

  bool try_override(style::sid sid,
                    std::invocable<style::transform &> auto &&call);

  style::sid fork(style::sid sid);

  style::sid freeze(style::sid sid);

  void replace_self_style(style::sid sid);

  style::value get(style::sid sid);
};

struct builder_state_interface : protected virtual builder_base {
  builder_state_interface(environment::tmp &tenv_, environment::persist &penv_,
                          insert_iterator it_, struct builder &builder_)
      : builder_base{penv_, tenv_, it_, builder_} {}

  bool has(units::uid uid, state::value s);

  void attach(units::uid uid, state::value s);

  void detach(units::uid uid, state::value s);

  void persist(units::uid uid);
};

export struct builder final : protected virtual builder_base,
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
  builder(environment::persist &penv, environment::tmp &tenv,
          insert_iterator it) noexcept
      : builder_base{penv, tenv, it, *this},
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

    builder b{penv, tenv, {tree.root()}};

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

  ++deep_index.at(current_deep).position;
  auto nit = it.at(sketch::value_t{
      .layout = &layout,
      .uid = ait ? ait->uid : units::uid{0},
      .sid = sid_,
      .order = ait ? units::ui::order{global_index++, ait->order.layer}
                   : units::ui::order{global_index++, 0}});

  std::swap(nit, it);

  ++current_deep;
  if (deep_index.size() == current_deep) {
    deep_index.push_back({});
  }
  call(builder);
  ++deep_index.at(current_deep).branch;
  deep_index.at(current_deep).position = 0;
  --current_deep;
  std::swap(nit, it);
};

void builder_element_interface::frame(units::uid uid_, style::sid sid_,
                                      const layout::frame &layout_,
                                      builder_block_cpt auto &&call) noexcept {
  auto ait = tree::access_iterator{it};

  ++deep_index.at(current_deep).position;
  auto nit = it.at(sketch::value_t{
      .layout = &layout_,
      .uid = uid_,
      .sid = sid_,
      .order = ait ? units::ui::order{global_index++, ait->order.layer}
                   : units::ui::order{global_index++, 0}});

  std::swap(nit, it);
  ++current_deep;
  if (deep_index.size() == current_deep) {
    deep_index.push_back({});
  }
  call(builder);
  ++deep_index.at(current_deep).branch;
  deep_index.at(current_deep).position = 0;
  --current_deep;
  std::swap(nit, it);
};

void builder_element_interface::frame(units::uid uid_, style::sid sid_,
                                      const layout::frame &layout_) noexcept {
  auto ait = tree::access_iterator{it};

  ++deep_index.at(current_deep).position;
  auto nit = it.at(sketch::value_t{
      .layout = &layout_,
      .uid = uid_,
      .sid = sid_,
      .order = ait ? units::ui::order{global_index++, ait->order.layer}
                   : units::ui::order{global_index++, 0}});
};

void builder_element_interface::frame(style::sid sid_,
                                      const layout::frame &layout_) noexcept {

  auto ait = tree::access_iterator{it};

  ++deep_index.at(current_deep).position;
  auto nit = it.at(sketch::value_t{
      .layout = &layout_,
      .uid = ait ? ait->uid : units::uid{0},
      .sid = sid_,
      .order = ait ? units::ui::order{global_index++, ait->order.layer}
                   : units::ui::order{global_index++, 0}});
};

void builder_element_interface::text(const iuic::text::raw::token &token,
                                     iuic::style::sid sid_,

                                     const layout::text &layout_) {
  auto ait = tree::access_iterator{it};
  text(token, (ait ? ait->uid : units::uid{0}), sid_, layout_);
};

void builder_element_interface::text(const iuic::text::raw::token &token,
                                     iuic::units::uid uid_,
                                     iuic::style::sid sid_,
                                     const layout::text &layout_) {
  auto ait = tree::access_iterator{it};

  // if not insert ?
  ++deep_index.at(current_deep).position;
  auto nit = it.at(sketch::value_t{
      .layout = &layout_,
      .uid = uid_,
      .sid = sid_,
      .order = ait ? units::ui::order{global_index++, ait->order.layer}
                   : units::ui::order{global_index++, 0},
      .text = {&token, 1} // SINGLE TOKEN SPAN
  });
};

void builder_element_interface::text(
    std::span<const iuic::text::raw::token> tokens, iuic::style::sid sid_,
    const layout::text &layout_) {
  auto ait = tree::access_iterator{it};

  ++deep_index.at(current_deep).position;
  auto nit = it.at(sketch::value_t{
      .layout = &layout_,
      .uid = ait ? ait->uid : units::uid{0},
      .sid = iuic::style::sid{0},
      .order = ait ? units::ui::order{global_index++, ait->order.layer}
                   : units::ui::order{global_index++, 0},
      .text = tokens

  });
};
// ---- IMPL [uid] ----

units::uid
builder_uid_interface::make_static(const std::string &str,
                                   const utils::anchor &anchor) const noexcept {

  std::stringstream ss;
  ss << anchor.value;
  ss << str;
  return units::uid{
      static_cast<std::uint64_t>(std::hash<std::string>{}(ss.str()))};
};

units::uid builder_uid_interface::make_ordered(const std::string &str,
                                               const utils::anchor &anchor) {
  static thread_local char buff[512]{"--unique--"}; // type prefix

  std::size_t size{10};

  std::size_t order[3]{0, 0, 0};

  order[0] = current_deep;
  order[1] = deep_index.at(current_deep).position;
  order[2] = deep_index.at(current_deep).branch;

  std::memcpy(&buff[size], &order, sizeof(order));
  size += sizeof(order);
  std::memcpy(&buff[size], &anchor.value, sizeof(order));
  size += sizeof(anchor.value);
  std::memcpy(&buff[size], str.data(), str.size());
  size += str.size();
  // make_ordered();
  // make_stable();

  std::size_t hash = std::hash<std::string_view>{}({buff, size});

  return units::uid{static_cast<std::uint64_t>(hash)};
};

units::uid builder_uid_interface::make_stable(const std::string &str,
                                              const utils::anchor &anchor) {
  static thread_local char buff[512]{"--stable--"}; // type prefix
  std::size_t size{10};

  if (auto ait = tree::access_iterator{it}) {
    std::memcpy(&buff[size], &ait->uid, sizeof(units::uid));
    size += sizeof(units::uid);
  } else {
    std::memcpy(&buff[size], &size, sizeof(size));
    size += sizeof(size);
  }

  std::memcpy(&buff[size], str.data(), str.size());
  size += str.size();

  std::memcpy(&buff[size], &anchor.value, sizeof(anchor.value));
  size += sizeof(anchor.value);

  std::size_t hash = std::hash<std::string_view>{}({buff, size});

  return units::uid{static_cast<std::uint64_t>(hash)};
};

units::uid builder_uid_interface::self() const noexcept {
  return tree::access_iterator{it}->uid;
};

// ---- IMPL [state] ----

void builder_state_interface::attach(units::uid uid, state::value v) {
  penv.state.access(uid).attach(v);
};

void builder_state_interface::detach(units::uid uid, state::value v) {
  penv.state.access(uid).detach(v);
};
bool builder_state_interface::has(units::uid uid, state::value v) {
  return penv.state.access(uid).has(v);
};

void builder_state_interface::persist(units::uid uid) {
  penv.state.access(uid).update_lifetime();
};

// ---- IMPL [event] ----

// ---- IMPL [style] ----

style::sid builder_style_interface::self() {
  return tree::access_iterator{it}->sid;
};

void builder_style_interface::replace_self_style(style::sid sid) {
  tree::access_iterator{it}->sid = sid;
};

style::sid builder_style_interface::make(const style::decl *s) {
  return tenv.style.make(s);
};

style::sid builder_style_interface::make(const style::decl &s) {
  return tenv.style.make(s);
};

// throw exception
void builder_style_interface::override(
    style::sid sid, std::invocable<style::shape &> auto &&call) {
  if (not tenv.style.override(sid, std::forward<decltype(call)>(call))) {
    throw iuic::exception::try_override_freeze{};
  }
};

// throw exception
void builder_style_interface::override(
    style::sid sid, std::invocable<style::decoration &> auto &&call) {
  if (not tenv.style.override(sid, std::forward<decltype(call)>(call))) {
    throw iuic::exception::try_override_freeze{};
  }
};

// throw exception
void builder_style_interface::override(
    style::sid sid, std::invocable<style::transform &> auto &&call) {
  if (not tenv.style.override(sid, std::forward<decltype(call)>(call))) {
    throw iuic::exception::try_override_freeze{};
  }
};

bool builder_style_interface::try_override(
    style::sid sid, std::invocable<style::shape &> auto &&call) {
  return tenv.style.override(sid, std::forward<decltype(call)>(call));
};

bool builder_style_interface::try_override(
    style::sid sid, std::invocable<style::decoration &> auto &&call) {
  return tenv.style.override(sid, std::forward<decltype(call)>(call));
};

bool builder_style_interface::try_override(
    style::sid sid, std::invocable<style::transform &> auto &&call) {
  return tenv.style.override(sid, std::forward<decltype(call)>(call));
};

style::sid builder_style_interface::fork(style::sid sid) {
  return tenv.style.fork(sid);
};

style::sid builder_style_interface::freeze(style::sid sid) {
  return tenv.style.freeze(sid);
};

style::value builder_style_interface::get(style::sid sid) {
  return tenv.style.get(sid);
};

// ---- IMPL [memory] ----
template <typename T> T &builder_memory_interface::tmp(T &&value) {
  T &ptr = tenv.memory.allocate<T>()[0];
  new (&ptr) T{std::move(value)};
  return ptr;
};

template <typename T>
void builder_memory_interface::persist(units::uid uid, std::type_identity<T>) {
  const erasure::type *type{erasure::type::from<T>()};

  auto access = penv.object.access(uid, type);

  if (auto state = access.state();
      state == environment::object_state::non_exist) {
    access.reserve();
  } else if (state == environment::object_state::alive_this_type) {
    access.update_lifetime();
  }
};

template <typename T>
bool builder_memory_interface::try_visit(units::uid uid,
                                         std::invocable<T &> auto &&call) {
  return erasure::visited::as_mutable{
      penv.object.access(uid, erasure::type::from<T>()).get()}
      .try_visit(std::forward<decltype(call)>(call));
};

void builder_memory_interface::init_if_not(units::uid uid,
                                           std::invocable<> auto &&call) {
  using traits = typename decltype(erasure::func_type{call})::traits;

  using type = std::remove_cvref_t<typename traits::return_t>;

  auto access = penv.object.access(uid, erasure::type::from<type>());

  if (auto state = access.state();
      state == environment::object_state::reserve_this_type) {
    access.allocate();
    access.construct([&](void *mem) { new (mem) type{call()}; });
  }
};

// ---- IMPL [text] ----
const text::raw::token &
builder_text_interface::static_token(std::string_view text,
                                     std::string_view font) {
  struct data {
    text::raw::token token;
    std::vector<iuic::text::glyph::id_t> memory;
  };

  static text::raw::token inv{.atlas_id = text::atlas::invalid_id};
  static std::unordered_map<text::atlas::id,
                            std::unordered_map<std::string, data>>
      cache;
  auto &atlas = text::atlas::by_name(font);
  auto id = atlas.get_id();

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
builder_text_interface::dynamic_token(std::string_view text,
                                      std::string_view font) {
  static text::raw::token inv{.atlas_id = text::atlas::invalid_id};

  auto &atlas = text::atlas::by_name(font);

  if (atlas.get_id() == text::atlas::invalid_id || text.size() > 50) {
    return inv;
  }

  if (atlas.decoder->capabilities().is_std_char_support()) {
    auto res = atlas.decoder->decode(text); // exceptions

    text::raw::token tk{.atlas_id = atlas.get_id(), .glyphs = res};

    text::raw::token &tk_mem = tenv.memory.allocate<text::raw::token>()[0];

    auto gl_mem = tenv.memory.allocate<text::glyph::id_t>(res.size());

    if (not gl_mem.empty()) {
      for (std::size_t i{0}; i < res.size(); ++i) {
        gl_mem[i] = res[i];
      }
    }

    tk_mem.atlas_id = atlas.get_id();
    tk_mem.glyphs = gl_mem;

    return tk_mem;
  }

  return inv;
};
// ---- IMPL [policy] ----

void builder_policy_interface::set(units::uid uid, auto p) {
  tenv.policy.set(uid, p);
};

void builder_policy_interface::set(auto p) {
  set(tree::access_iterator{it}->uid, p);
};

template <typename T> T builder_policy_interface::get(units::uid uid) {
  return tenv.policy.get<T>(uid);
};

template <typename T> T builder_policy_interface::get() {
  return get<T>(tree::access_iterator{it}->uid);
};
}; // namespace iuic::scheme
