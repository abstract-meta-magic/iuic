// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator;
import std;

export namespace iuic::iterator {

template <typename Container, typename... Ex> struct common;

template <typename Container, typename Mode, typename... Ex> struct underlying;

template <typename T>
concept is_mode = requires {
  T::container;
  T::basic;
};

template <typename T>
concept is_const_drop_allowed = true;

template <typename T>
concept is_container = true;

template <typename T>
concept has_sentinel = requires { typename T::sentinel; };

template <typename T>
concept has_unwrap =
    true; // TODO: broken with (this auto&& self) CRTP Potected cast

template <typename T>
concept has_const_unwrap = requires { T::unwrap_as_const(); };

template <typename T>
concept has_getptr = requires { T::getptr_as_mutable(); };

template <typename T>
concept has_const_getptr = requires { T::getptr_as_const(); };

template <typename T>
concept is_iterable = true;

template <typename T>
concept is_valid_basic_iterator = true;

template <typename T>
concept has_increment = requires(T obj) { obj.increment(); };

template <typename T>
concept has_decrement = requires(T obj) { obj.decrement(); };

template <typename T>
concept is_valid_underlying = requires(T obj) {
  typename T::container_type;
  typename T::value_type;
} && has_increment<T>;

template <typename L, typename R>
concept is_underlying_assignable =
    requires(L &lhs, const R &rhs) { lhs.assign(rhs); };

namespace mode {

struct tag_t {};

struct basic_t : tag_t {

} basic;

struct liner_t : tag_t {
} liner{};

}; // namespace mode

namespace extension {

struct tag_t {};

template <template <typename...> typename Iterator> struct iterator : tag_t {};

struct debug_t : tag_t {
} debug;
}; // namespace extension

// TODO: using underlying::sentinel ??
template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
struct sentinel : protected underlying<T, Mode, Ex...> {

  using underlying = underlying<T, Mode, Ex...>;

  sentinel(underlying u) : underlying{u} {};

  constexpr bool operator==(const underlying &other) const {
    return this->equal(other);
  };
};

template <is_iterable T, typename Mode, typename... Ex>
  requires has_sentinel<underlying<T, Mode, Ex...>>
struct sentinel<T, Mode, Ex...>
    : protected underlying<T, Mode, Ex...>::sentinel {
  using underlying = underlying<T, Mode, Ex...>;

  sentinel(underlying::sentinel s) : underlying::sentinel{s} {};

  sentinel(underlying u)
    requires requires { typename underlying::sentinel{u}; }
      : underlying::sentinel{u} {};

  sentinel()
    requires requires { typename underlying::sentinel{}; }
      : underlying::sentinel{} {};

  constexpr bool operator==(const underlying &other) const {
    return other.equal(static_cast<const underlying::sentinel &>(*this));
  };
};

template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
  requires(is_valid_underlying<underlying<T, Mode, Ex...>>)
struct basic : protected underlying<T, Mode, Ex...> {

  template <is_iterable Other, typename OMode, typename... OEx>
    requires(is_valid_underlying<underlying<Other, OMode, OEx...>>)
  friend struct basic;

  using underlying = underlying<T, Mode, Ex...>;

  template <is_iterable Other, typename OMode, typename... OEx>
  basic(basic it, const basic<Other, OMode, OEx...> &other) : basic{it} {
    *this = other;
  };

  template <is_iterable Other, typename OMode, typename... OEx>
  basic(basic<Other, OMode, OEx...> other)
      : underlying{
            static_cast<basic<Other, OMode, OEx...>::underlying &>(other)} {};

  template <is_iterable Other, typename OMode>
  basic(basic<Other, OMode, Ex...> other, Mode)
      : underlying{static_cast<basic<Other, OMode>::underlying &>(other)} {};

  template <is_iterable Other, typename OMode, typename... OEx>
  basic(basic<Other, OMode, OEx...> other, Mode, Ex...)
      : underlying{static_cast<basic<Other, OMode>::underlying &>(other)} {};

  basic(underlying u) : underlying{u} {};

  operator bool() const {
    return *this != sentinel{static_cast<const underlying &>(*this)};
  };

  operator sentinel<T, Mode, Ex...>() const {
    return sentinel<T, Mode, Ex...>{static_cast<const underlying &>(*this)};
  };

  constexpr basic &operator++() {
    this->increment();
    return *this;
  }

  constexpr basic &operator--()
    requires requires { this->decrement(); }
  {
    this->decrement();
    return *this;
  }

  constexpr basic operator++(int) {
    auto res = *this;
    ++res;
    return res;
  }

  constexpr basic operator--(int)
    requires requires { this->decrement(); }
  {
    auto res = *this;
    --res;
    return res;
  }

  template <typename OC, typename OM, typename... OEx>
  constexpr bool operator==(const basic<OC, OM, OEx...> &other) const {
    return this->equal(other);
  };

  constexpr bool operator==(const sentinel<T, Mode, Ex...> &other) const {
    return other == static_cast<const underlying &>(*this);
  };

  template <typename OC, typename OM, typename... OEx>
  constexpr basic &operator=(const basic<OC, OM, OEx...> &other) {
    this->assign(other);
    return *this;
  };

  // TODO: work
  constexpr std::size_t operator-(const basic &other) {
    return this->difference(other);
  };

  // TODO: work
  constexpr std::size_t operator-(const sentinel<T, Mode> &other) {
    return this->difference(other);
  };

  template <typename OMode>
  basic<T, OMode, Ex...> operator=(OMode m) const
    requires std::is_base_of_v<mode::tag_t, OMode>
  {
    return basic<T, OMode, Ex...>{*this};
  };

  basic<T, Mode> drop_ex() const { return basic<T, Mode>{*this}; };
};

template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
  requires(is_valid_basic_iterator<basic<T, Mode, Ex...>>)
struct access : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  access(basic b) : basic{b} {};

  template <is_iterable Other, typename OMode, typename... OEx>
  access(iterator::basic<Other, OMode, OEx...> other)
      : basic{std::move(other)} {};

  constexpr decltype(auto) operator*()
    requires has_unwrap<typename basic::underlying>
  {
    return static_cast<basic::underlying &>(*this).unwrap_as_mutable();
  };

  constexpr decltype(auto) operator->()
    requires has_getptr<typename basic::underlying>
  {
    return static_cast<basic::underlying &>(*this).getptr_as_mutable();
  }
};

template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
  requires(is_valid_basic_iterator<basic<T, Mode, Ex...>>)
struct const_access : private basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  const_access(basic b) : basic{b} {};

  constexpr decltype(auto) operator*()
    requires has_const_unwrap<typename basic::underlying>
  {
    return static_cast<basic::underlying &>(*this).unwrap_as_const();
  };

  constexpr decltype(auto) operator->()
    requires has_const_getptr<typename basic::underlying>
  {
    return static_cast<basic::underlying &>(*this).getptr_as_const();
  }
};

template <is_iterable T, typename Mode, typename... Ex>
  requires(is_valid_basic_iterator<basic<T, Mode, Ex...>> &&
           basic<T, Mode, Ex...>::underlying::allow_const_drop == true)
struct const_access<T, Mode, Ex...> : public basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  const_access(basic b) : basic{b} {};

  constexpr auto operator*()
    requires requires { basic::underlying::get_constant_value_reference(); }
  {
    return this->get_constant_value_reference();
  };

  constexpr auto operator->()
    requires requires { basic::underlying::get_constant_value_pointer(); }
  {
    return this->get_constant_value_pointer();
  }
};

template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
  requires(is_valid_basic_iterator<basic<T, Mode, Ex...>>)
struct move : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  move(basic b) : basic{b} {};

  constexpr decltype(std::move(basic::underlying::value_ref)) operator*() {
    return std::move(this->get_value_reference());
  };
};

template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
  requires(is_valid_basic_iterator<basic<T, Mode, Ex...>>)
struct copy : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  copy(basic b) : basic{b} {};

  constexpr basic::underlying::const_value_ref operator*() {
    return this->get_constant_value_reference();
  };
};

template <is_iterable T, typename Mode = mode::liner_t, typename... Ex>
  requires(is_valid_basic_iterator<basic<T, Mode, Ex...>>)
struct insert : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  insert(basic b) : basic{b} {};

  [[nodiscard]]
  insert operator&(auto &&...args)
    requires requires {
      basic::underlying::insert_at(std::forward<decltype(args)>(args)...);
    }
  {
    typename basic::underlying res{*this};
    res.insert_at(std::forward<decltype(args)>(args)...);
    return basic{std::move(res)};
  };

  insert &operator&=(auto &&...args) &
    requires requires {
      basic::underlying::insert_at(std::forward<decltype(args)>(args)...);
    }
  {
    this->insert_at(std::forward<decltype(args)>(args)...);
    return *this;
  };

  [[nodiscard]] insert operator<<(auto &&...args)
    requires requires {
      basic::underlying::insert_before(std::forward<decltype(args)>(args)...);
    }
  {
    typename basic::underlying res{*this};
    res.insert_before(std::forward<decltype(args)>(args)...);
    return basic{std::move(res)};
  };

  insert &operator<<=(auto &&...args) &
    requires requires {
      basic::underlying::insert_before(std::forward<decltype(args)>(args)...);
    }
  {
    this->insert_before(std::forward<decltype(args)>(args)...);
    return *this;
  };

  [[nodiscard]] insert operator>>(auto &&...args)
    requires requires {
      basic::underlying::insert_after(std::forward<decltype(args)>(args)...);
    }
  {
    typename basic::underlying res{*this};
    res.insert_after(std::forward<decltype(args)>(args)...);
    return basic{std::move(res)};
  };

  insert &operator>>=(auto &&...args) &
    requires requires {
      basic::underlying::insert_after(std::forward<decltype(args)>(args)...);
    }
  {
    this->insert_after(std::forward<decltype(args)>(args)...);
    return *this;
  };

  [[nodiscard]] insert operator()(auto &&...args)
    requires requires {
      basic::underlying::insert_in(std::forward<decltype(args)>(args)...);
    }
  {
    typename basic::underlying res{*this};
    res.insert_in(std::forward<decltype(args)>(args)...);
    return basic{std::move(res)};
  };

  insert &operator[](auto &&...args) &
    requires requires {
      basic::underlying::insert_in(std::forward<decltype(args)>(args)...);
    }
  {
    this->insert_in(std::forward<decltype(args)>(args)...);
    return *this;
  };
};

template <typename T, typename Mode, typename... Ex>
basic(underlying<T, Mode, Ex...>) -> basic<T, Mode, Ex...>;

template <typename T, typename Mode, typename OMode, typename... Ex>

basic(basic<T, Mode, Ex...>, OMode) -> basic<T, OMode, Ex...>;
template <typename T, typename Mode, typename OMode, typename... Ex,
          typename... OEx>

basic(basic<T, Mode, Ex...>, OMode, OEx...) -> basic<T, OMode, OEx...>;

template <typename T, typename Mode, typename... Ex>
access(basic<T, Mode, Ex...>) -> access<T, Mode, Ex...>;
template <typename T, typename Mode, typename... Ex>
const_access(basic<T, Mode, Ex...>) -> const_access<T, Mode, Ex...>;
template <typename T, typename Mode, typename... Ex>
move(basic<T, Mode, Ex...>) -> move<T, Mode, Ex...>;
template <typename T, typename Mode, typename... Ex>
copy(basic<T, Mode, Ex...>) -> copy<T, Mode, Ex...>;

namespace mode {
struct reverse_t : tag_t {
} reverse{};

struct index_t : tag_t {
} index{};

struct iterator_t : tag_t {
} iterator{};

struct root_t : tag_t {
} root{};

struct sibling_t : tag_t {
} sibling{};

}; // namespace mode

namespace range {

struct basic_t {
} basic;

struct reverse_t {
} reverse;

template <typename C, typename M, typename... Ex> struct spec {
  struct undefined {};
};

template <typename C, typename M = basic_t, typename... Ex>
spec<C, M, Ex...>::range from(C &&container, M mode = basic, Ex... extensions) {
  return spec<C, M>::make(std::forward<C>(container), mode, extensions...);
};

// basic spec
template <typename C, typename Mode, typename... Ex>
struct spec<iterator::basic<C, Mode, Ex...>, basic_t> {
  using basic_iterator = iuic::iterator::basic<C, Mode, Ex...>;
  using access_iterator = iuic::iterator::access<C, Mode, Ex...>;

  struct range {
    range(basic_iterator it) : begin_{it} {}

    auto begin() { return access_iterator{begin_}; };

    auto end() { return iuic::iterator::sentinel{begin()}; }

  private:
    basic_iterator begin_;
  };

  static range make(iuic::iterator::basic<C, Mode, Ex...> it,
                    iuic::iterator::range::basic_t) {
    return range{it};
  };
};

}; // namespace range
}; // namespace iuic::iterator
