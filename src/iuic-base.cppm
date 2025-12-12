

module;

#include <chrono>
#include <concepts>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

// Independ User Interface Core

export module iuic.core:base;

namespace iuic {
void advance(auto &);

template <typename T> consteval auto remove_all_pointer() {
  if constexpr (std::is_pointer_v<T>) {
    return remove_all_pointer<std::remove_pointer_t<T>>();
  } else {
    return std::type_identity<T>{};
  }
}

}; // namespace iuic

export namespace iuic {

template <typename T>
concept defer_call_cpt =
    std::is_nothrow_destructible_v<T> && std::is_nothrow_invocable_v<T> &&
    (std::is_nothrow_copy_constructible_v<T> ||
     std::is_nothrow_move_constructible_v<T>);

template <defer_call_cpt T> struct defer {
  using type = std::remove_cvref_t<T>;

  defer(const defer &) = delete;
  defer(defer &&) = delete;
  defer &operator=(const defer &) = delete;
  defer &operator=(defer &&) = delete;
  ~defer() noexcept {
    if (not canceled) {
      d();
    }
  }

  constexpr void cancel() noexcept { canceled = true; };

  template <defer_call_cpt S>
  constexpr defer(S &&d_) noexcept : d{std::forward<S>(d_)} {}

private:
  type d;
  bool canceled{false};
};

template <typename T> defer(T &&) -> defer<T>;

template <typename T> struct virtual_iterator {
  virtual ~virtual_iterator() = default;

  virtual constexpr void prev() noexcept = 0;

  virtual constexpr void next() noexcept = 0;

  virtual constexpr bool valid() const noexcept = 0;

  virtual T *get() noexcept = 0;

  virtual_iterator &operator++() noexcept { next(); };

  virtual_iterator &operator--() noexcept { prev(); };

  T &operator*() noexcept { return *get(); };

  T *operator->() noexcept { return get(); };

  struct sentinel_t {};

  sentinel_t sentinel() const noexcept { return {}; };

  constexpr operator bool() const noexcept { return valid(); };

  constexpr bool operator==(const sentinel_t &) const noexcept {
    return not valid();
  };

  constexpr bool operator!=(const sentinel_t &) const noexcept {
    return valid();
  };

  struct iterator_wrapper {
    iterator_wrapper(virtual_iterator *ptr_) : ptr{ptr_} {};

    iterator_wrapper &operator++() {
      ptr->next();
      return *this;
    };

    iterator_wrapper &operator--() {
      ptr->prev();
      return *this;
    };

    T &operator*() { return *ptr->get(); };

    T *operator->() { return ptr->get(); };

    constexpr operator bool() const { return ptr->valid(); };

    constexpr bool operator==(const sentinel_t &) const {
      return ptr->valid();
    };

    constexpr bool operator!=(const sentinel_t &) const {
      return not ptr->valid();
    };

  private:
    virtual_iterator *ptr;
  };

  struct range_adapter {
    range_adapter(virtual_iterator *ptr_) : ptr{ptr_} {}

    iterator_wrapper begin() { return {ptr}; };

    sentinel_t end() { return {}; };

  private:
    virtual_iterator *ptr;
  };
  range_adapter range() { return {this}; };
};

struct invalid_virtual_iterator {
  template <typename T> struct iterator : virtual_iterator<T> {
    void next() override {};
    void prev() override {};
    bool valid() const override { return false; };
    T &get() override {
      throw std::logic_error{"Try get invalide iterator object"};
    };
  };

  template <typename T> operator std::unique_ptr<virtual_iterator<T>>() {
    return std::unique_ptr<virtual_iterator<T>>{new iterator<T>{}};
  };
};

struct ctype_base {
  const ctype_base *const self{this};
};

template <auto seed = {}> struct ctype : ctype_base {
  consteval ctype() = default;

  consteval explicit ctype(const ctype &base) noexcept
      : ctype_base{}, base{&base} {}

  constexpr bool operator==(const ctype &other) const noexcept {
    return self == other.self;
  };

  operator std::size_t() const noexcept {
    return reinterpret_cast<std::size_t>(self);
  };

  std::size_t type_id() const noexcept { return *this; };

  constexpr bool base_of(const ctype &other) const noexcept {

    const ctype *current = static_cast<const ctype *>(self);

    for (;;) {
      auto &_ = *current;
      if (_ == other) {
        return true;
      } else if (_.self == _.base) {
        break;
      }

      current = current->base;
    }

    return false;
  };

  const ctype *const base{this};
};

template <auto decl__ = []() {}> consteval decltype(auto) anonim_tag() {
  struct {
  } decl;
  return decl;
};

using extern_type = ctype<anonim_tag()>;

constexpr inline extern_type extern_null{};

struct extern_binding {

  constexpr virtual ~extern_binding() = default;

  constexpr virtual const extern_type &type() const noexcept {
    return extern_null;
  };

  constexpr virtual std::string_view info() const noexcept {
    return "Extern binding interface";
  };
};

template <typename T>
using pure_t =
    std::remove_cvref_t<typename decltype(remove_all_pointer<T>())::type>;

template <typename T>
concept is_pure =
    std::same_as<std::remove_cvref_t<T>, T> && not std::is_pointer_v<T>;

namespace unit {
struct percent {};
}; // namespace unit

// using hash_t
// using srk
using uid_t = std::uint64_t;
namespace uid {
struct anchor {
  consteval anchor() = default;
  anchor(const anchor &) = delete;
  anchor &operator=(const anchor &) = delete;
  anchor(anchor &&) = delete;
  anchor &operator=(anchor &&) = delete;
};
}; // namespace uid
using pixel_t = std::int32_t;
using upixel_t = std::uint32_t;
using time_t = std::chrono::time_point<std::chrono::steady_clock>;
using time_duration_t = std::chrono::duration<double>;

// type for angl

struct percent_t {
  constexpr auto operator<=>(const percent_t &) const = default;

  constexpr percent_t(float value_) noexcept
      : value{value_ >= 0 ? (value_ < 300 ? value_ : 300) : 0} {}

  constexpr operator float() const noexcept { return value * 0.01; }

private:
  float value;
};

struct vh_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vh_t &) const = default;

  constexpr vh_t(const percent_t &p) noexcept : value{p} {}
  constexpr vh_t(percent_t &&p) noexcept : value{p} {}
  constexpr vh_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vh_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct vw_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vw_t &) const = default;

  constexpr vw_t(const percent_t &p) noexcept : value{p} {}
  constexpr vw_t(percent_t &&p) noexcept : value{p} {}
  constexpr vw_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vw_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct em_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const em_t &) const = default;

  constexpr em_t(const percent_t &p) noexcept : value{p} {}
  constexpr em_t(percent_t &&p) noexcept : value{p} {}
  constexpr em_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr em_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct rem_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const rem_t &) const = default;

  constexpr rem_t(const percent_t &p) noexcept : value{p} {}
  constexpr rem_t(percent_t &&p) noexcept : value{p} {}
  constexpr rem_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr rem_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};
// need px,%,rem,vh,vw

struct ui_position {
  pixel_t x, y = 0;
  constexpr auto operator<=>(const ui_position &) const = default;
};

struct z_order_t {
  std::uint16_t group;
  std::uint16_t priority;
};

// width | hieght
struct ui_size {
  upixel_t w, h = 0;
  constexpr auto operator<=>(const ui_size &) const = default;
};

struct ui_rect {
  ui_position position;
  ui_size size;
  std::tuple<pixel_t, pixel_t, upixel_t, upixel_t> xywh() const {
    return {position.x, position.y, size.w, size.h};
  };
  constexpr auto operator<=>(const ui_rect &) const = default;
};

struct color_t {
  static constexpr color_t get_white() noexcept {
    return {255, 255, 255, 255};
  };

public:
  uint8_t r{0}, g{0}, b{0}, a = {255};
};

struct ui_none {};

struct ui_inherit {};

struct ui_auto {};

struct ui_background_image {};

using style_background = std::variant<ui_none, color_t, ui_background_image>;

using ui_adaptive_unit =
    std::variant<ui_auto, upixel_t, percent_t, vw_t, vh_t, rem_t>;

struct indent {
  ui_adaptive_unit top{upixel_t{0}}, bottom{upixel_t{0}}, left{upixel_t{0}},
      right{upixel_t{0}};
};

struct border_radius {
  // TODO : body
};

struct ui_adaptive_size {
  ui_adaptive_unit w{upixel_t{0}};
  ui_adaptive_unit h{upixel_t{0}};
};

struct aspect_ratio {
  std::uint8_t width{0};
  std::uint8_t height{0};
  enum dominant_side_e { AUTO, WIDTH, HEIGHT } dominant_side{AUTO};
};

// text font
struct upm_t {
  //...
};

} // namespace iuic
