

module;

export module iuic.core:kernel;
import std;
import :base;
import :style;
import :state;
import :policy;
import :layout.def;

namespace iuic::kernel {

struct violated_order : std::runtime_error {
  violated_order()
      : std::runtime_error{
            "The computing order of iuic::text_layout or "
            "iuic::frame_layout within iuic::computing_context for "
            "iuic::celement is broken. This is a core library bug. If you see "
            "this message, your release of the library is broken."} {}
};

struct element {
  std::uint16_t self;
  std::uint16_t brother;
  std::uint16_t parent;
  enum meta_state : std::uint16_t {
    null = 0,
    alive = 1 << 1,
    root = 1 << 2,
    root_child = 1 << 3,
    text = 1 << 4,
    discarded = 1 << 5,
    request = 1 << 6,
    arrange = 1 << 7,
  } meta;
};

constexpr element::meta_state operator|(element::meta_state lhs,
                                        element::meta_state rhs) {
  return element::meta_state{static_cast<std::uint16_t>(
      static_cast<std::uint16_t>(lhs) | static_cast<std::uint16_t>(rhs))};
}

constexpr element::meta_state operator&(element::meta_state lhs,
                                        element::meta_state rhs) {
  return element::meta_state{static_cast<std::uint16_t>(
      static_cast<std::uint16_t>(lhs) & static_cast<std::uint16_t>(rhs))};
}

constexpr bool operator&&(element::meta_state lhs, element::meta_state rhs) {
  auto eq = lhs & rhs;
  return eq == rhs;
}

constexpr element::meta_state &operator|=(element::meta_state &lhs,
                                          element::meta_state rhs) {
  lhs = lhs | rhs;
  return lhs;
}

constexpr element::meta_state &operator&=(element::meta_state &lhs,
                                          element::meta_state rhs) {
  lhs = lhs & rhs;
  return lhs;
}

constexpr element::meta_state operator^(element::meta_state lhs,
                                        element::meta_state rhs) {
  return element::meta_state{static_cast<std::uint16_t>(
      static_cast<std::uint16_t>(lhs) ^ static_cast<std::uint16_t>(rhs))};
}

constexpr element::meta_state operator~(element::meta_state lhs) {
  return element::meta_state{
      static_cast<std::uint16_t>(~static_cast<std::uint16_t>(lhs))};
}

// Дле представления используется
// Альтернативная блочная модель
struct request {
  request_size size;
  element element;
};

struct state_model {
  virtual void attach(iuic::uid_t, iuic::state) = 0;

  virtual void detach(iuic::uid_t, iuic::state) = 0;

  // replace to std::ranges::view
  virtual std::unique_ptr<virtual_iterator<const iuic::state>>
      get(iuic::uid_t, iuic::state) const = 0;

  virtual bool is_exist(iuic::uid_t) const = 0;

  virtual bool update_livetime(iuic::uid_t) const = 0;

  virtual bool has(iuic::uid_t, iuic::state) const = 0;
};

struct userspace {
  virtual ~userspace() = default;

  virtual std::expected<ui_rect, int>
      get_rect_bordered(element) const noexcept = 0;

  virtual std::expected<ui_rect, int>
      get_rect_borderless(element) const noexcept = 0;

  virtual std::unique_ptr<virtual_iterator<const request>>
  get_requests(element, bool reverse = false) const noexcept = 0;

  virtual std::variant<const frame_layout *, const text_layout *>
      get_layout(element) const noexcept = 0;

  virtual element get_parent(element) = 0;

  virtual std::unique_ptr<virtual_iterator<const element>>
  get_childs(element, bool reverse = false) const noexcept = 0;

  virtual std::expected<const style::cref *, int>
      get_style(element) const noexcept = 0;

  virtual std::expected<z_order_t, int> get_zorder(element) const noexcept = 0;

  virtual std::expected<iuic::uid_t, int> get_uid(element) const noexcept = 0;

  virtual std::expected<policy::hovered, int>
      get_hovered_policy(element) const noexcept = 0;

  virtual std::expected<policy::event, int>
      get_event_policy(element) const noexcept = 0;

  // Default - Предок всегда перед потомком
  // Reverse - Потомок всегда перед предком
  // Нет Discarded элементов
  // Нет элементов без alive
  // может добавить ordered = false, для прохода согласно zorderd
  virtual std::unique_ptr<virtual_iterator<const element>>
  get_elements(bool reverse = false) const = 0;

  // if nothin selected return null element
  virtual element get_selected() const noexcept = 0;

  virtual state_model *state() = 0;

  virtual const state_model *state() const = 0;

  virtual std::uint64_t hash(std::span<const std::byte>) const = 0;
};

struct memory_model {
  struct type {
    template <is_pure T> static const type *from() {

      if constexpr (requires() {
                      { T::livetime } -> std::convertible_to<std::size_t>;
                    }) {
        static constexpr type res{&res,
                                  std::is_trivially_destructible_v<T>,
                                  sizeof(T),
                                  alignof(T),
                                  T::livetime,
                                  [](const void *const obj) static {
                                    delete static_cast<const T *const>(obj);
                                  }};
        return &res;
      } else {
        static constexpr type res{&res,
                                  std::is_trivially_destructible_v<T>,
                                  sizeof(T),
                                  alignof(T),
                                  0,
                                  [](const void *const obj) static {
                                    delete static_cast<const T *const>(obj);
                                  }};
        return &res;
      }
    };

    static const type *none() {
      struct _ {};
      return from<_>();
    };

    const void *const id;
    const bool trivial_dctor;
    const std::size_t size;
    const std::size_t align;
    const std::size_t livetime; //  in frames
    void (*const dctor)(const void *const);

  private:
    constexpr type(const void *const i, bool td, std::size_t s, std::size_t a,
                   std::size_t lt, void (*const d)(const void *const)) noexcept
        : id{i}, trivial_dctor{td}, size{s}, align{a}, livetime{lt},
          dctor{d} {};
  };

  enum class object_state : std::uint8_t {
    none_exist,
    reserve_none_type,
    reserve_other_type,
    reserve_this_type,
    alive_other_type,
    alive_this_type,
    garbage_other_type,
    garbage_this_type
  };

  virtual ~memory_model() = default;

  // Заререзвировать объект.
  // Возможны преаллокации.
  virtual void reserve(iuic::uid_t, const type * = type::none()) noexcept = 0;

  // Если объект reserve_none | reserve_this аллацировать память.
  // Если объект alive_this, то вернуть его локацию.
  // В иных случаях вернуть nullptr.
  virtual void *locate(iuic::uid_t, const type *) noexcept = 0;

  virtual object_state state(iuic::uid_t,
                             const type * = type::none()) const noexcept = 0;

  virtual bool update_livetime(iuic::uid_t) const noexcept = 0;

  virtual void launch(iuic::uid_t, const type *) noexcept = 0;

  virtual bool as(iuic::uid_t, const type *) const noexcept = 0;

  virtual void *tmp(const type *, std::size_t count = 1) noexcept = 0;
};

struct root : userspace {
  virtual ~root() = default;

  // create and select new element
  virtual element instance(iuic::uid_t, const frame_layout *,
                           style::ref) noexcept = 0;

  // create and select new element
  virtual element instance(iuic::uid_t, const text_layout *,
                           style::ref) noexcept = 0;

  // make element alive and select parent
  virtual element launch(element) noexcept = 0;

  virtual element discard(element) noexcept = 0;

  virtual bool validate() const = 0;

  virtual void override(element, style::decoration *) noexcept = 0;

  virtual void override(element, style::shape *) noexcept = 0;

  virtual void override(element, style::transform *) noexcept = 0;

  virtual void override(element, z_order_t) noexcept = 0;

  virtual void override(element, policy::hovered) noexcept = 0;

  virtual void override(element, policy::event) noexcept = 0;

  virtual memory_model *memory() = 0;

  virtual const memory_model *memory() const = 0;
};

struct hardware : root {
  virtual ~hardware() = default;

  virtual void attach(element, request_size) noexcept = 0;

  virtual void apply(element, ui_rect bordered) noexcept = 0;

  virtual void apply(element, ui_rect bordered,
                     ui_rect borderless) noexcept = 0;

  virtual void advance() noexcept = 0;
};

using kernel_ctor_cpt = std::unique_ptr<hardware> (*)() noexcept;

std::unique_ptr<hardware> default_kernel() noexcept;
} // namespace iuic::kernel
