// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.core:kernel;
import std;
import iuic.underlying;
import iuic.state;
import :style;
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
  std::uint16_t self{0};
  std::uint16_t brother{0};
  std::uint16_t parent{0};
  struct meta_tag {
    enum value_t : std::uint16_t {
      null = 0,
      alive = 1 << 1,
      root = 1 << 2,
      root_child = 1 << 3,
      first_child = 1 << 4,
      last_child = 1 << 5,
      text = 1 << 6,
      discarded = 1 << 7,
      request = 1 << 8,
      arrange = 1 << 9,
    };

  private:
    struct query_proxy {
      constexpr query_proxy &&
      any_of(std::same_as<value_t> auto... flags) && noexcept {
        if (result) {
          result = ((value & flags) || ...) != 0;
        }

        return std::move(*this);
      };

      constexpr query_proxy &&
      none_of(std::same_as<value_t> auto... flags) && noexcept {
        if (result) {
          result = ((value & flags) && ...) == 0;
        };

        return std::move(*this);
      };

      constexpr operator bool() const noexcept { return result; };

      constexpr query_proxy(value_t value_, bool result_)
          : value{value_}, result{result_} {};

    private:
      meta_tag::value_t value;
      bool result;
    };

  public:
    constexpr meta_tag &attach(std::same_as<value_t> auto... flags) noexcept {
      value = static_cast<value_t>((std::to_underlying(value) | ... | flags));

      return *this;
    };

    constexpr meta_tag &detach(std::same_as<value_t> auto... flags) noexcept {
      value = static_cast<value_t>(std::to_underlying(value) & ~(... | flags));
      return *this;
    };

    constexpr meta_tag &merge(const meta_tag &other) noexcept {
      value = static_cast<value_t>(std::to_underlying(value) |
                                   std::to_underlying(other.value));
      return *this;
    };

    constexpr bool strong_equal(const meta_tag &other) const noexcept {
      return value == other.value;
    };

    constexpr bool
    strong_equal(std::same_as<value_t> auto... flags) const noexcept {
      return strong_equal(meta_tag{flags...});
    };

    constexpr query_proxy weak_equal(const meta_tag &other) const noexcept {
      return query_proxy{value, (value & other.value) == other.value};
    };

    constexpr query_proxy
    weak_equal(std::same_as<value_t> auto... flags) const noexcept {
      return weak_equal(meta_tag{flags...});
    };

    constexpr query_proxy has(value_t flag) const noexcept {
      return query_proxy{value, (value & flag) != 0};
    };

    constexpr query_proxy
    any_of(std::same_as<value_t> auto... flags) const noexcept {
      return query_proxy{value, ((value & flags) || ...) != 0};
    };

    constexpr query_proxy
    none_of(std::same_as<value_t> auto... flags) const noexcept {
      return query_proxy{value, ((value & flags) && ...) == 0};
    };

    constexpr void reset() noexcept { value = null; };

    explicit constexpr meta_tag(std::same_as<value_t> auto... flags) noexcept {
      attach(flags...);
    }

    constexpr meta_tag() noexcept : value{null} {};

  private:
    value_t value{null};
  } meta_info{};

  explicit constexpr element(std::same_as<meta_tag::value_t> auto... flags)
      : self{0}, brother{0}, parent{0}, meta_info{flags...} {}

  constexpr element(std::uint16_t self_, std::uint16_t brother_,
                    std::uint16_t parent_, meta_tag meta_info_ = {}) noexcept
      : self{self_}, brother{brother_}, parent{parent_}, meta_info{meta_info_} {
  }

  constexpr element() : self{0}, brother{0}, parent{0}, meta_info{} {}
};

constexpr element::meta_tag operator|(const element::meta_tag &lhs,
                                      const element::meta_tag &rhs) noexcept {
  auto copy = lhs;
  copy.merge(rhs);
  return copy;
};

constexpr element::meta_tag operator|(const element::meta_tag &lhs,
                                      element::meta_tag::value_t rhs) noexcept {
  auto copy = lhs;
  copy.attach(rhs);
  return copy;
};

constexpr element::meta_tag operator|(element::meta_tag::value_t lhs,
                                      const element::meta_tag &rhs) noexcept {
  auto copy = rhs;
  copy.attach(lhs);
  return copy;
};

constexpr element::meta_tag operator|(element::meta_tag::value_t lhs,
                                      element::meta_tag::value_t rhs) noexcept {
  return element::meta_tag{lhs, rhs};
};

constexpr bool operator==(const element::meta_tag &lhs,
                          const element::meta_tag &rhs) noexcept {
  return lhs.strong_equal(rhs);
};

constexpr bool operator!=(const element::meta_tag &lhs,
                          const element::meta_tag &rhs) noexcept {
  return not(lhs == rhs);
};

// Дле представления используется
// Альтернативная блочная модель
struct request {
  request_size size;
  element element;
};

struct state_model {
  virtual void attach(units::uid, iuic::state::value) = 0;

  virtual void detach(units::uid, iuic::state::value) = 0;

  // replace to std::ranges::view
  virtual std::unique_ptr<utils::virtual_iterator<const iuic::state::value>>
      get(units::uid, iuic::state::value) const = 0;

  virtual bool is_exist(units::uid) const = 0;

  virtual bool update_livetime(units::uid) const = 0;

  virtual bool has(units::uid, iuic::state::value) const = 0;
};

struct userspace {
  virtual ~userspace() = default;

  virtual std::expected<units::ui::rect, int>
      get_rect_bordered(element) const noexcept = 0;

  virtual std::expected<units::ui::rect, int>
      get_rect_borderless(element) const noexcept = 0;

  virtual std::unique_ptr<utils::virtual_iterator<const request>>
  get_requests(element, bool reverse = false) const noexcept = 0;

  virtual std::variant<const frame_layout *, const text_layout *>
      get_layout(element) const noexcept = 0;

  virtual element get_parent(element) = 0;

  virtual std::unique_ptr<utils::virtual_iterator<const element>>
  get_childs(element, bool reverse = false) const noexcept = 0;

  virtual std::expected<const style::cref *, int>
      get_style(element) const noexcept = 0;

  virtual std::expected<units::z_order_t, int>
      get_zorder(element) const noexcept = 0;

  virtual std::expected<units::uid, int> get_uid(element) const noexcept = 0;

  virtual std::expected<policy::hovered, int>
      get_hovered_policy(element) const noexcept = 0;

  virtual std::expected<policy::event, int>
      get_event_policy(element) const noexcept = 0;

  // Default - Предок всегда перед потомком
  // Reverse - Потомок всегда перед предком
  // Нет Discarded элементов
  // Нет элементов без alive
  // может добавить ordered = false, для прохода согласно zorderd
  virtual std::unique_ptr<utils::virtual_iterator<const element>>
  get_elements(bool reverse = false) const = 0;

  // if nothin selected return null element
  virtual element get_selected() const noexcept = 0;

  virtual state_model *state() = 0;

  virtual const state_model *state() const = 0;

  virtual std::uint64_t hash(std::span<const std::byte>) const = 0;

  // dupm в tree::node_type
};

struct memory_model {
  enum class object_state : std::uint8_t {
    none_exist,
    reserve_none_type,
    reserve_other_type,
    reserve_this_type,
    alive_other_type,
    alive_this_type,
    outdated_other_type,
    outdated_this_type,
    garbage_other_type,
    garbage_this_type
  };

  virtual ~memory_model() = default;

  // Заререзвировать объект.
  // Возможны преаллокации.
  virtual void
  reserve(units::uid,
          const erasure::type * = erasure::type::none()) noexcept = 0;

  // Если объект reserve_none | reserve_this аллацировать память.
  // Если объект alive_this, то вернуть его локацию.
  // В иных случаях вернуть nullptr.
  virtual void *locate(units::uid, const erasure::type *) noexcept = 0;

  virtual object_state
  state(units::uid,
        const erasure::type * = erasure::type::none()) const noexcept = 0;

  virtual bool update_livetime(units::uid) const noexcept = 0;

  virtual void launch(units::uid, const erasure::type *) noexcept = 0;

  virtual bool as(units::uid, const erasure::type *) const noexcept = 0;

  virtual void *tmp(const erasure::type *, std::size_t count = 1) noexcept = 0;
};

struct root : userspace {
  virtual ~root() = default;

  // create and select new element
  virtual element instance(units::uid, const frame_layout *,
                           style::ref) noexcept = 0;

  // create and select new element
  virtual element instance(units::uid, const text_layout *,
                           style::ref) noexcept = 0;

  // make element alive and select parent
  virtual element launch(element) noexcept = 0;

  virtual element discard(element) noexcept = 0;

  virtual bool validate() const = 0;

  virtual void override(element, style::decoration *) noexcept = 0;

  virtual void override(element, style::shape *) noexcept = 0;

  virtual void override(element, style::transform *) noexcept = 0;

  virtual void override(element, units::z_order_t) noexcept = 0;

  virtual void override(element, policy::hovered) noexcept = 0;

  virtual void override(element, policy::event) noexcept = 0;

  virtual memory_model *memory() = 0;

  virtual const memory_model *memory() const = 0;
};

struct hardware : root {
  virtual ~hardware() = default;

  virtual void attach(element, request_size) noexcept = 0;

  virtual void apply(element, units::ui::rect bordered) noexcept = 0;

  virtual void apply(element, units::ui::rect bordered,
                     units::ui::rect borderless) noexcept = 0;

  virtual void advance() noexcept = 0;
};

using kernel_ctor_cpt = std::unique_ptr<hardware> (*)() noexcept;

std::unique_ptr<hardware> default_kernel() noexcept;
} // namespace iuic::kernel

export std::ostream &operator<<(std::ostream &out,
                                const iuic::kernel::element &e) {
  using meta_tag = iuic::kernel::element::meta_tag;
  out << "iuic::kernel::element {" << std::endl;
  out << "  self    : " << e.self << ";" << std::endl;
  out << "  brother : " << e.brother << ";" << std::endl;
  out << "  parent  : " << e.parent << ";" << std::endl;
  out << "  meta    : [";
  out << "r:" << (e.meta_info.weak_equal(meta_tag::root) ? "*" : "o") << "|";
  out << "rc:" << (e.meta_info.weak_equal(meta_tag::root_child) ? "*" : "o")
      << "|";
  out << "t:" << (e.meta_info.weak_equal(meta_tag::text) ? "*" : "o") << "|";
  out << "al:" << (e.meta_info.weak_equal(meta_tag::alive) ? "*" : "o") << "|";
  out << "rq:" << (e.meta_info.weak_equal(meta_tag::request) ? "*" : "o")
      << "|";
  out << "arr:" << (e.meta_info.weak_equal(meta_tag::arrange) ? "*" : "o")
      << "|";
  out << "D:" << (e.meta_info.weak_equal(meta_tag::discarded) ? "*" : "o")
      << "]" << std::endl;
  out << "}" << std::endl;

  return out;
};
