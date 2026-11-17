// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:range;
import std;
import :base;
import :it.basic;
import :it.access;

export namespace iuic::iterator::range {

struct basic_t {
} basic;

struct reverse_t {
} reverse;

template <typename C, typename M, typename... Ex> struct spec {
  struct undefined {};
};

template <typename C, typename M = basic_t, typename... Ex>
spec<C, M, Ex...>::range from(C &&container, M mode = basic, Ex... extensions) {
  return spec<C, M, Ex...>::make(std::forward<C>(container), mode,
                                 extensions...);
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

}; // namespace iuic::iterator::range
