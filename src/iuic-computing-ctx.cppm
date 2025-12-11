

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

export module iuic.core:computing.kernal;
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
  std::uint16_t broder;
  std::uint16_t parent;
  enum : std::uint16_t {
    null = 0,
    root = 1 << 1,
    root_child = 1 << 2,
    text = 1 << 3,
    discarded = 1 << 4,
    request = 1 << 5,
    arrange = 1 << 6,
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
};

struct kernel_root : kernel_user {
  virtual ~kernel_root() = default;

  virtual void override(element, style::decoration) noexcept;

  virtual void override(element, style::shape) noexcept;

  virtual void override(element, style::transform) noexcept;

  virtual void override(element, z_order_t) noexcept;

  virtual void override(element, policy::hovered) noexcept;

  virtual void override(element, policy::event) noexcept;
};

struct kernel_hardware : kernel_root {
  virtual ~kernel_hardware() = default;

  virtual void attach(element, request_size) noexcept;

  virtual void apply(element, ui_rect bordered) noexcept;

  virtual void apply(element, ui_rect bordered, ui_rect borderless) noexcept;
};
} // namespace iuic::computing
