// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.utils:decl;
import std;
import iuic.underlying.cenv;
import iuic.underlying.erasure;

export namespace iuic {

template <typename T> struct container {
  static_assert(false, "Has no impl");
};
}; // namespace iuic

export namespace iuic::utils {

constexpr void assert(std::invocable<> auto &&call,
                      std::string_view msg = "No diagnostic",
                      std::source_location source = {}) {
  if constexpr (iuic::cenv::logic("iuic::debug.assert").value_or(false)) {
    if (not call()) {
      throw std::logic_error{std::format("{}:{} :: Assertion failure :: {}",
                                         source.file_name(), source.line(),
                                         msg)};
    };
  }
};

struct deduction_context final {};

template <typename T>
concept is_deduction_context =
    std::same_as<std::remove_cvref_t<T>, deduction_context>;

template <typename T,
          bool = iuic::cenv::logic("iuic::debug.api").value_or(false)>
struct debug_api;

template <typename T> struct debug_api<T, false> {};

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

// this is UB struct.
template <erasure::is_pure_type Owner, std::size_t offset = 0>
struct adv_member_for {
  constexpr adv_member_for() {}

protected:
  constexpr Owner &self() {
    if constexpr (offset == 0) {
      return *static_cast<Owner *>(static_cast<void *>(this));
    } else {
      void *adv_ptr = static_cast<char *>(static_cast<void *>(this)) - offset;
      return *static_cast<Owner *>(adv_ptr);
    }
  };

  constexpr const Owner &self() const {
    if constexpr (offset == 0) {
      return *static_cast<const Owner *>(static_cast<const void *>(this));
    } else {
      const void *adv_ptr = static_cast<const char *>(this) - offset;
      return *static_cast<const Owner *>(adv_ptr);
    }
  };
};

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

template <typename Char, std::size_t N> struct str {
  constexpr str(const Char (&data_)[N]) {
    for (std::size_t i{0}; i < N; ++i) {
      data[i] = data_[i];
    }
  };

  constexpr operator std::basic_string_view<Char>() const {
    return std::basic_string_view<Char>{data};
  };

  template <std::size_t ON>
  constexpr bool operator==(const Char (&other)[ON]) const {
    return std::basic_string_view<Char>{data} ==
           std::basic_string_view<Char>{other};
  };

  Char data[N];
  static constexpr std::size_t size{N - 1};
};

}; // namespace ct
}; // namespace iuic::utils

namespace iuic::utils {

// for fun ?
template <std::size_t N, typename CharType, bool is_null_term = true,
          bool use_char_size = false>
struct fstring_base {
  template <std::size_t ON>
    requires(ON <= N)
  constexpr fstring_base(const CharType (&e)[ON]){};

  std::basic_string_view<CharType> to_view() const {
    return std::basic_string_view<CharType>{&data[0], N};
  };

  template <std::size_t ON>
  fstring_base<ON + N, CharType, is_null_term>
  operator+(const fstring_base<ON, CharType, is_null_term> &);

private:
  std::array<CharType, is_null_term ? N + 1 : N> data;
  std::uint8_t size;
};

using fstring = fstring_base<31, char, false>;
using flstring = fstring_base<63, char, false>;
}; // namespace iuic::utils

// unique
namespace iuic::utils {
struct unique_id_t {};

template <typename UNIQUE = decltype([]() {})>
static constexpr unique_id_t unique_id{};

template <typename T, auto &id> struct unique_index {};

template <typename T> struct is_unique_index_impl : std::false_type {};

template <typename T, auto &id>
struct is_unique_index_impl<unique_index<T, id>> : std::true_type {};

template <typename T>
concept is_unique_indext = is_unique_index_impl<T>::value;

export template <is_unique_indext T> struct unique_type {
  template <typename U = decltype([] {}), auto &id = unique_id<>>
  constexpr unique_type(unique_index<U, id> = {}) {};
};

template <typename U = decltype([] {}), auto &id = unique_id<>>
unique_type(unique_index<U, id>) -> unique_type<unique_index<U, id>>;

static_assert(
    not std::same_as<decltype(unique_type{{}}), decltype(unique_type{{}})>,
    "unique type is broken");

template <unique_type t = {{}}> constexpr decltype(t) test__() { return {}; };

static_assert(not std::same_as<decltype(test__()), decltype(test__())>,
              "default NTTP unique type is broken");

}; // namespace iuic::utils
