// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:impl.shift.bfs;
import :flat.hierarchy.bfs;
import :flat.bfs;

export namespace iuic::tree {

base_iterator<hierarchy::bfs> shift(base_iterator<hierarchy::bfs> from,
                                    base_iterator<hierarchy::bfs> to) {
  // UNSAFE
  // NEED TO CHECK HIERARCHY
  struct : decltype(from) {
    auto extract() { return this->owner; };
  } extract_form{from};

  struct : decltype(to) {
    auto extract() { return this->self; };
  } extract_to{to};

  return {extract_to.extract(), extract_form.extract()};
};

template <typename T, typename U>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> from,
                                      base_iterator<flat_bfs_type<U>> to) {
  // UNSAFE
  // NEED TO CHECK HIERARCHY
  struct : decltype(from), decltype(to) {
    decltype(from) value() {
      return decltype(from){decltype(to)::self, decltype(from)::owner};
    };
  } shift{from, to};

  return shift.value();
};

template <typename T>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> from,
                                      base_iterator<flat_bfs_type<T>> to) {
  // UNSAFE
  // NEED TO CHECK HIERARCHY
  struct : decltype(from) {
    auto extract() { return this->owner; };
  } extract_form{from};

  struct : decltype(to) {
    auto extract() { return this->to; };
  } extract_to{to};

  return {extract_to.extract(), extract_form.extract()};
};

template <typename T>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> from,
                                      base_iterator<hierarchy::bfs> to) {
  struct : decltype(from), decltype(to) {
    decltype(from) value() {
      return decltype(from){decltype(to)::self, decltype(from)::owner};
    };
  } shift{from, to};

  return shift.value();
};

} // namespace iuic::tree
