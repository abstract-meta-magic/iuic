

module;

#include <cstdint>
#include <exception>
#include <expected>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:computing.kernel;
import :base;
import :style;
import :policy;
import :layout.def;

namespace iuic::computing {

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
  enum : std::uint16_t {
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

// Дле представления используется
// Альтернативная блочная модель
struct request {
  request_size size;
  element element;
};

struct kernel_user {
  virtual ~kernel_user() = default;

  virtual std::expected<ui_rect, int> get_rect(element) const noexcept;

  virtual std::vector<request> get_requests(element) const noexcept;

  virtual std::variant<const frame_layout *, const text_layout *>
      get_layout(element) const noexcept;

  virtual element get_parent(element);

  virtual std::vector<element> get_childs(element) const noexcept;

  virtual std::expected<const style::cref *, int>
      get_style(element) const noexcept;

  virtual std::expected<z_order_t, int> get_zorder(element) const noexcept;

  virtual std::expected<iuic::uid_t, int> get_uid(element) const noexcept;

  virtual std::expected<policy::hovered, int>
      get_hovered_policy(element) const noexcept;

  virtual std::expected<policy::event, int>
      get_event_policy(element) const noexcept;
  virtual std::span<const element> range_for() const;

  // if nothin selected return null element
  virtual element get_selected() const noexcept;
};

// tmp-object
// present-object

struct object {

  void destruct();

private:
};

struct memory_model {
  using key = std::uint64_t;

  struct type {

    template <is_pure T> static const type *from() {
      static constexpr type res{
          &res, std::is_trivially_destructible_v<T>, sizeof(T), alignof(T),
          [](void *obj) static { delete static_cast<T *>(obj); }};
      return &res;
    };

    const void *const id;
    const bool trivial_dctor;
    const std::size_t size;
    const std::size_t align;
    void (*const dctor)(void *);
  };

  virtual ~memory_model() = default;

  virtual key make_key(std::span<const std::byte>) const;

  virtual void *persist(key, const type *);

  virtual bool is_init(key) const;

  virtual bool update_livetime(key) const;

  virtual bool is_exist(key) const;

  virtual bool as(key, const type *) const;

  virtual void *tmp(const type *, size_t count);
};

struct kernel_root : kernel_user {
  virtual ~kernel_root() = default;

  // select new element
  virtual element instance(const frame_layout *, style::ref) noexcept;

  // select new element
  virtual element instance(const text_layout *, style::ref) noexcept;

  // select parent
  virtual element launch(element) noexcept;

  virtual element discard(element) noexcept;

  virtual bool validate() const;

  virtual void reset() noexcept;

  virtual void override(element, style::decoration *) noexcept;

  virtual void override(element, style::shape *) noexcept;

  virtual void override(element, style::transform *) noexcept;

  virtual void override(element, z_order_t) noexcept;

  virtual void override(element, policy::hovered) noexcept;

  virtual void override(element, policy::event) noexcept;

  virtual memory_model *memory();

  virtual const memory_model *memory() const;
};

struct kernel_hardware : kernel_root {
  virtual ~kernel_hardware() = default;

  virtual void attach(element, request_size) noexcept;

  virtual void apply(element, ui_rect bordered) noexcept;

  virtual void apply(element, ui_rect bordered, ui_rect borderless) noexcept;
};
} // namespace iuic::computing
