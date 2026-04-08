// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying:utils;
import :erasure;
import std;

export namespace iuic::utils {

struct unimplemented : std::runtime_error {
  unimplemented()
      : std::runtime_error{"This part/function doesn't have implementation."} {
        };

  unimplemented(std::string msg)
      : std::runtime_error{std::format(
            "This part/function doesn't have implementation. msg : {}", msg)} {
        };
};

template <erasure::is_pure_type Owner> struct member_for {
  member_for(Owner &owner_) : owner{owner_} {}

protected:
  Owner &self() { return owner; };

  const Owner &self() const { return owner; };

private:
  Owner &owner;
};

template <erasure::is_pure_type Owner, std::size_t offset = 0>
struct adv_member_for {
  adv_member_for() {}

protected:
  Owner &self() {
    Owner *ptr =
        reinterpret_cast<Owner *>(reinterpret_cast<char *>(this) - offset);
    return *ptr;
  };

  const Owner &self() const {
    return *(reinterpret_cast<const Owner *>(
        reinterpret_cast<const char *>(this) - offset));
  };
};

// TODO : make full
template <typename T, typename Inner>
consteval auto get_type_of(const Inner T::*) -> Inner;

template <auto v> struct type_of_impl {
  using type = decltype(get_type_of(v));
};

template <auto i> using type_of = type_of_impl<i>::type;

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
/// ---------- SWAP BUFFERS ---------- ///
template <typename T, typename Container>
concept buffer_value_mover_cpt =
    requires {
      typename T::container_type;
      typename T::key_type;
    } &&
    requires(T &obj, typename T::container_type &c,
             const typename T::key_type &key, typename T::move_type value) {
      obj.move_to(c, obj.move_from(c, key));
    } &&
    erasure::is_pure_type<Container> &&
    std::same_as<Container, typename T::container_type> &&
    std::is_default_constructible_v<T>;

template <typename T> struct swap_buffer_traits;

template <typename T, std::size_t N,
          buffer_value_mover_cpt<T> Mover =
              typename swap_buffer_traits<T>::mover_type>
  requires(N > 1)
struct swap_buffers {
  auto get_buffers() {
    struct _ {
      T &prev;
      T &current;
    };

    return _{buffers_[prev], buffers_[current]};
  };

  auto get_buffers() const {
    struct _ {
      const T &prev;
      const T &current;
    };

    return _{buffers_[prev], buffers_[current]};
  };

  void swap() {
    prev = (prev + 1) % N;
    current = (current + 1) % N;

    swap_buffer_traits<T>::clear(buffers_[current]);
  };

  void move_forward(typename Mover::key_type key) const {
    Mover m;
    m.move_to(buffers_[current], m.move_from(buffers_[prev], key));
  };

private:
  std::int32_t prev{0};
  std::int32_t current{1};
  mutable std::array<T, N> buffers_;
};

template <typename Map> struct forward_for_map {
  using key_type = typename Map::key_type;
  using container_type = Map;
  using move_type = std::optional<typename Map::node_type>;

  static move_type move_from(Map &v, const key_type &key) {
    if (v.contains(key)) {
      return v.extract(key);
    }

    return std::nullopt;
  };

  static void move_to(Map &v, move_type swap) {
    if (swap) {
      v.insert(std::move(swap.value()));
    }
  };
};

template <typename T, typename Key>
struct swap_buffer_traits<std::unordered_map<T, Key>> {
  using mover_type = forward_for_map<std::unordered_map<T, Key>>;

  static constexpr void clear(std::unordered_map<T, Key> &buff) {
    buff.clear();
  };
};

namespace ct {

template <typename T, std::size_t N> struct index_tree {
  static constexpr std::size_t invalide_index{
      std::numeric_limits<std::size_t>::max()};

  constexpr index_tree(std::array<T, N> &&data_) : data{std::move(data_)} {}

  constexpr std::size_t get_index(const T &value) const {

    if (auto it = std::find(data.begin(), data.end(), value);
        it != std::end(data)) {
      return std::distance(data.begin(), it);
    };

    return invalide_index;
  };

  constexpr std::size_t size() const { return N; };

private:
  std::array<T, N> data;
};
template <typename T, T... value> struct list;

template <> struct list<void> {
  template <typename T, T... i2>
  constexpr list<T, i2...> operator&(list<T, i2...>) {
    return {};
  }

  constexpr decltype(auto) operator&(auto call) { return list{}; }

  constexpr list<void> operator&(list<void>) { return {}; }
};

template <typename T, T... value> struct list {
  template <T... i2>
  constexpr list<T, value..., i2...> operator&(list<T, i2...>) {
    return {};
  }

  constexpr list<T, value...> operator&(list<void>) { return {}; }

  constexpr decltype(auto) operator&(std::invocable<list> auto call) {
    return call(*this);
  }

  static constexpr list get() { return {}; };

  static constexpr decltype(auto) expand(auto call) {
    return call.template operator()<value...>();
  };
};

template <std::size_t N> struct str {
  char cstr[N];
  constexpr std::string_view to_view() const { return {cstr}; };
};

}; // namespace ct
}; // namespace iuic::utils
