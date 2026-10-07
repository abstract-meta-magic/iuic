// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:free_list;
import std;
import :base;

namespace iuic::asset::free_list {

template <typename Value, std::size_t S> struct fixed {};
}; // namespace iuic::asset::free_list

namespace iuic {

template <typename Value, std::size_t S>
struct container<iuic::asset::free_list::fixed<Value, S>> {
private:
  union slot {
    std::size_t next{0};
    Value value;
    ~slot() {};
  };
  static constexpr std::size_t end_{S};

public:
  container() {
    for (std::size_t i{0}; i < S; ++i) {
      data[i].next = i + 1;
    };
  };

  ~container() {
    for (std::size_t i{0}; i < S; ++i) {
      if (occuped.test(i)) {
        std::destroy_at(&data[i].value);
      };
    };
  };

  template <typename T> std::size_t insert(T &&value_) {
    // if head != end
    if (head != end_) {
      auto &head_slot = data[head];

      auto next = head_slot.next;

      try {
        // replace to std::construct ???
        new (&head_slot.value) T{std::forward<T>(value_)};
      } catch (...) {
        head_slot.next = next;
        throw;
      }
      occuped.flip(head);
      std::swap(head, next);
      return next;
    };
    return end_;
  };

  void erase(std::size_t I) {
    if (occuped.test(I)) {
      auto &this_slot = data[I];
      std::destroy_at(std::addressof(this_slot.value));
      this_slot.next = head;
      head = I;
      occuped.flip(I);
    };
  };

  Value &at(std::size_t I) {
    if (occuped.test(I)) {
      return data[I].value;
    };
    throw std::logic_error{"try get uninitialized variable"};
  };

  const Value &at(std::size_t I) const {
    if (occuped.test(I)) {
      return data.at(I).value;
    };
    throw std::logic_error{"try get uninitialized variable"};
  };

  bool has_value(std::size_t I) { return occuped.test(I); };

  bool full() const { return head == end_; };

  struct iterator {};

  // iterator begin();
  // iterator end();

private:
  slot data[S]{}; // not std::array -> union
  std::bitset<S> occuped{};
  std::size_t head{0};
};

}; // namespace iuic
