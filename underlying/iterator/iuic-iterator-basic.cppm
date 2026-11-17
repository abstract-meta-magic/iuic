// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:it.basic;
import std;
import :base;
import :underlying_table;
import :sentinel;

export namespace iuic::iterator {

template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct basic : protected underlying<T, Mode, Ex...> {
private:
  template <typename Other, typename OMode, typename... OEx>
  friend struct basic;

protected:
  using underlying = underlying<T, Mode, Ex...>;

public: // Basic operator's
  operator bool() const {
    return *this != sentinel{static_cast<const underlying &>(*this)};
  };

  operator sentinel<T, Mode, Ex...>() const {
    return sentinel<T, Mode, Ex...>{static_cast<const underlying &>(*this)};
  };

  constexpr basic &operator++() {
    underlying_table::increment(*this);
    return *this;
  }

  constexpr basic &operator--()
    requires requires { this->decrement(); }
  {
    underlying_table::decrement(*this);
    return *this;
  }

  constexpr basic operator++(int) {
    auto res = *this;
    ++res;
    return res;
  }

  constexpr basic operator--(int) {
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
    underlying_table::assign(*this, other);
    return *this;
  };

  template <typename OMode>
  basic<T, OMode, Ex...> operator=(OMode m) const
    requires std::is_base_of_v<mode::tag_t, OMode>
  {
    return basic<T, OMode, Ex...>{*this};
  };

  basic<T, Mode> drop_ex() const { return basic<T, Mode>{*this}; };

public: // CTORS
  template <typename Other, typename OMode, typename... OEx>
  basic(basic &&it, const basic<Other, OMode, OEx...> &other)
      : basic{std::move(it)} {
    *this = other;
  };

  template <typename Other, typename OMode, typename... OEx>
  basic(const basic &it, const basic<Other, OMode, OEx...> &other) : basic{it} {
    *this = other;
  };

  template <typename Other, typename OMode, typename... OEx>
  basic(const basic<Other, OMode, OEx...> &other)
      : underlying{static_cast<const basic<Other, OMode, OEx...>::underlying &>(
            other)} {};

  template <typename Other, typename OMode, typename... OEx>
  basic(basic<Other, OMode, OEx...> &&other)
      : underlying{
            static_cast<basic<Other, OMode, OEx...>::underlying &&>(other)} {};

  template <typename Other, typename OMode>
  basic(const basic<Other, OMode, Ex...> &other, Mode)
      : underlying{
            static_cast<const basic<Other, OMode>::underlying &>(other)} {};

  template <typename Other, typename OMode>
  basic(basic<Other, OMode, Ex...> &&other, Mode)
      : underlying{static_cast<basic<Other, OMode>::underlying &&>(other)} {};

  template <typename Other, typename OMode, typename... OEx>
  basic(const basic<Other, OMode, OEx...> &other, Mode, Ex...)
      : underlying{
            static_cast<const basic<Other, OMode>::underlying &>(other)} {};

  template <typename Other, typename OMode, typename... OEx>
  basic(basic<Other, OMode, OEx...> &&other, Mode, Ex...)
      : underlying{static_cast<basic<Other, OMode>::underlying &&>(other)} {};

  basic(const underlying &u) : underlying{u} {};
  basic(underlying &&u) : underlying{std::move(u)} {};
};

template <typename T, typename Mode, typename... Ex>
basic(const underlying<T, Mode, Ex...> &) -> basic<T, Mode, Ex...>;

template <typename T, typename Mode, typename... Ex>
basic(underlying<T, Mode, Ex...> &&) -> basic<T, Mode, Ex...>;

template <typename T, typename Mode, typename OMode, typename... Ex>
basic(const basic<T, Mode, Ex...> &, OMode) -> basic<T, OMode, Ex...>;

template <typename T, typename Mode, typename OMode, typename... Ex>
basic(basic<T, Mode, Ex...> &&, OMode) -> basic<T, OMode, Ex...>;

template <typename T, typename Mode, typename OMode, typename... Ex,
          typename... OEx>
basic(const basic<T, Mode, Ex...> &, OMode, OEx...) -> basic<T, OMode, OEx...>;

template <typename T, typename Mode, typename OMode, typename... Ex,
          typename... OEx>
basic(basic<T, Mode, Ex...> &&, OMode, OEx...) -> basic<T, OMode, OEx...>;

}; // namespace iuic::iterator
