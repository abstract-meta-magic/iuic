

module;

// Independ User Interface Core

export module iuic.core:base;
import std;

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

  virtual void prev() noexcept = 0;

  virtual void next() noexcept = 0;

  virtual bool valid() const noexcept = 0;

  virtual T *get() noexcept = 0;

  virtual_iterator &operator++() noexcept { next(); };

  virtual_iterator &operator--() noexcept { prev(); };

  T &operator*() noexcept { return *get(); };

  T *operator->() noexcept { return get(); };

  struct sentinel_t {};

  sentinel_t sentinel() const noexcept { return {}; };

  operator bool() const noexcept { return valid(); };

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
      return not ptr->valid();
    };

    constexpr bool operator!=(const sentinel_t &) const {
      return ptr->valid();
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
    void next() noexcept override {};
    void prev() noexcept override {};
    bool valid() const noexcept override { return false; };
    T *get() noexcept override { return nullptr; };
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
concept as_pure_type =
    std::same_as<std::remove_cvref_t<T>, T> && not std::is_pointer_v<T>;

namespace unit {
struct percent {};
}; // namespace unit

// TODO : MOVE TO iuic.underline;
namespace erasure {
struct type {
  template <as_pure_type T> static const type *from() {

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
      : id{i}, trivial_dctor{td}, size{s}, align{a}, livetime{lt}, dctor{d} {};
};

template <typename T> struct is_function_signature : std::false_type {};

template <typename T> struct is_pure_function_signature : std::false_type {};

template <typename... ARGS> struct func_args {
  using as_tuple_t = std::tuple<ARGS...>;
  static constexpr auto size = sizeof...(ARGS);

  template <std::size_t i>
    requires(i <= sizeof...(ARGS))
  using arg_t = std::tuple_element_t<i, as_tuple_t>;
};

template <typename R, typename... ARGS>
struct is_function_signature<R(ARGS...)> : std::true_type {};

// cv cvalifier
template <typename R, typename... ARGS>
struct is_pure_function_signature<R(ARGS...)> : std::true_type {};

template <typename R, typename... ARGS>
struct is_function_signature<R (*)(ARGS...)>
    : is_function_signature<R(ARGS...)> {};

template <typename R, typename... ARGS>
struct is_function_signature<R (&)(ARGS...)>
    : is_function_signature<R(ARGS...)> {};

template <typename R, typename T, typename... ARGS>
struct is_function_signature<R (T::*)(ARGS...)>
    : is_function_signature<R(ARGS...)> {};

template <typename R, typename T, typename... ARGS>
struct is_function_signature<R (T::*)(ARGS...) const>
    : is_function_signature<R(ARGS...)> {};

template <typename T>
concept as_func_sig_cpt = is_function_signature<T>::value;

template <as_func_sig_cpt T> struct func_traits;

template <typename R, typename... ARGS> struct func_traits<R(ARGS...)> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;
};

template <typename R, typename... ARGS> struct func_traits<R (*)(ARGS...)> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;
};

template <typename R, typename T, typename... ARGS>
struct func_traits<R (T::*)(ARGS...)> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;
};

// TODO : Try support [int*&,int****&,int**&,int***] ref to pointer, pointret to
// pointer
template <typename T, typename D> struct like {
  static constexpr bool value =
      ((std::is_lvalue_reference_v<T> && std::is_lvalue_reference_v<D>) ||
       (std::is_rvalue_reference_v<T> && std::is_rvalue_reference_v<D>) ||
       (as_pure_type<std::remove_const_t<std::remove_volatile_t<T>>> &&
        as_pure_type<std::remove_const_t<std::remove_volatile_t<D>>>) ||
       (std::is_pointer_v<T> && std::is_pointer_v<D>)) &&
      (std::is_const_v<std::remove_volatile_t<std::remove_reference_t<T>>> ==
       std::is_const_v<std::remove_volatile_t<std::remove_reference_t<D>>>) &&
      (std::is_volatile_v<std::remove_const_t<std::remove_reference_t<T>>> ==
       std::is_volatile_v<std::remove_const_t<std::remove_reference_t<D>>>);
};

// TODO : other as_decoy methods for traits
template <typename R, typename T, typename... ARGS>
struct func_traits<R (T::*)(ARGS...) const> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;

  template <as_func_sig_cpt U> static constexpr bool as_decoy() {
    using other_trait = func_traits<U>;

    if constexpr (like<typename other_trait::return_t, return_t>::value) {

      if constexpr (other_trait::func_args::size == func_args::size) {
        auto f = []<typename... SELF_ARGS, typename... OTHER_ARGS>(
                     erasure::func_args<SELF_ARGS...>,
                     erasure::func_args<OTHER_ARGS...> other) static {
          return (like<SELF_ARGS, OTHER_ARGS>::value && ...);
        };

        return f(func_args{}, typename other_trait::func_args{});
      }
    }

    return false;
  };
};

struct decoy {};

template <as_func_sig_cpt T> struct func_type {
  using traits = func_traits<T>;

  constexpr func_type(T) {};

  constexpr func_type(auto &&heh) {};
};

func_type(auto t) -> func_type<decltype(&decltype(t)::operator())>;

template <func_type ft> struct funt_type_t {
  using traits = typename decltype(ft)::traits;
};

template <typename T, typename D>
concept func_as_decoy = requires(T call) {
  { func_type{call} };
  requires decltype(func_type{call})::traits::template as_decoy<D>();
};

struct visited {
  struct as_const;
  struct as_const_sync;
  struct as_mutable;
  struct as_mutable_sync;

  template <typename T> bool as() const noexcept;

  bool as(const type *) const noexcept;

  visited(void *data_, const type *type_) : data{data_}, type{type_} {};

private:
  void *data;
  const type *type;
};

struct visited::as_const : private visited {
  as_const(visited);
  template <typename T> as_const(T *data_) : visited{data_, type::from<T>()} {};

  as_const(std::nullptr_t) : visited{nullptr, type::none()} {};

  bool try_visit(func_as_decoy<decoy(const decoy &)> auto &&call) {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    if (type::from<pure_t<arg_t>>() == type) {
      call(*static_cast<arg_ptr>(data));
      return true;
    }
    return false;
  };

  bool visit_any(func_as_decoy<decoy(const decoy &)> auto &&...calls) {
    return (try_visit(std::forward<decltype(calls)>(calls)) || ...);
  };

  decltype(auto) visit_or(func_as_decoy<decoy(const decoy &)> auto &&call,
                          std::invocable<> auto call_or);
};

struct visited::as_mutable : private visited {
  as_mutable(visited);
  template <typename T>
  as_mutable(T *data_) : visited{data_, type::from<T>()} {};

  as_mutable(std::nullptr_t) : visited{nullptr, type::none()} {};

  bool try_visit(func_as_decoy<decoy(decoy &)> auto &&call) {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    if (type::from<pure_t<arg_t>>() == type) {
      call(*static_cast<arg_ptr>(data));
      return true;
    }

    return false;
  };

  decltype(auto) visit_or(auto, auto);

  visited::as_const as_const() const { return visited{*this}; };
};

void hehh(visited::as_mutable m) {
  m.try_visit([](std::vector<int> &vec) {
    // job
  });
}
}; // namespace erasure

// using hash_t
// using srk
using uid_t = std::uint64_t;
namespace uid {

struct anchor {
  template <typename T>
  anchor(T &obj) : value{(std::size_t)std::addressof(obj)} {};
  template <typename T>
  anchor(T *obj) : value{(std::size_t)std::addressof(obj)} {};
  anchor() : value{(std::size_t)std::addressof(*this)} {};
  anchor(std::size_t value_) : value{value_} {};

  //
  anchor(const anchor &) = default;
  anchor &operator=(const anchor &) = default;
  anchor(anchor &&) = default;
  anchor &operator=(anchor &&) = default;

  //
  std::size_t value;
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
  std::uint8_t r{0}, g{0}, b{0}, a = {255};
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
