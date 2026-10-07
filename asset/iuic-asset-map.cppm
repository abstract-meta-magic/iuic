// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:map;
import std;
import :base;

namespace iuic::asset::map {

template <typename T> struct hash_from {
  static constexpr hash_t operator()(const T &obj) {
    return std::hash<T>{}(obj);
  };
};

template <typename T>
concept key = requires(T obj) { obj == obj; };

template <typename T> struct traits {};

template <typename Key, typename Value, typename Allocator> struct dynamic {};

template <typename Key, typename Value, std::size_t S> struct fixed {};

}; // namespace iuic::asset::map

namespace iuic {
// load factor ????
template <typename Key, typename Value, std::size_t S>
struct container<iuic::asset::map::fixed<Key, Value, S>> {
  static_assert(std::has_single_bit(S), "ivalid size");
  struct slot {
    Key key{};
    std::optional<Value> value{std::nullopt};
  };

  struct accessor {
    accessor(Key key_, container *owner_)
        : key{key_}, owner{owner_}, index{0}, is_found{false} {
      find_();
    };

    accessor(const accessor &) = delete ("proxy struct");
    accessor(accessor &&) = delete ("proxy struct");
    accessor &operator=(const accessor &) = delete ("proxy struct");
    accessor &operator=(accessor &&) = delete ("proxy struct");

    Value &get() {
      // in to assert ?
      if (is_found) {
        return owner->data.at(index).value.value();
      }
      throw std::logic_error{"Access to unset variable"};
    };

    bool has_value() {
      if (is_found) {
        return owner->data.at(index).value.has_value();
      }
      return false;
    };

    // invalidate accessor
    template <typename T> bool set(T &&value) && {
      if (is_found) {
        owner->data.at(index).value.emplace(std::forward<T>(value));
        return true;
      } else {
        std::size_t index_ = index;
        for (std::size_t i{0}; i < 10; ++i) {
          if (auto &slot = owner->data.at(index_); not slot.value.has_value()) {
            slot.key = key;
            slot.value.emplace(std::forward<T>(value));
            std::println("set value");
            is_found = true;
            index = index_;
            return true;
          }

          index_ = (index_ + 1) & mask;
        };
      }
      return false;
    };

    // invalidate accessor
    void reset() && {
      if (is_found) {
        auto &slot = owner->data.at(index);

        slot.key = {};
        slot.value.reset();
        is_found = false;
      }
    };

  private:
    static constexpr auto mask = S - 1;

    void find_() {
      iuic::asset::map::hash_from<Key> hfk;
      iuic::asset::hash_t hash = hfk(key);
      index = hash & mask;
      for (std::size_t i{0}, index_{index}; i < 10;
           ++i, index_ = (index_ + 1) & mask) {
        if (auto &s = owner->data.at(index_); s.key == key) {
          index = index_;
          is_found = true;
          break;
        }
      }
    };

  private:
    Key key;
    container *owner;
    std::size_t index;
    bool is_found;
  };

  struct iterator {
    // value type is proxy
    // ++
    // --
    // ==
  private:
    container *owner;
    std::size_t index;
  };

  accessor access(Key key) { return accessor{key, this}; };

  iterator begin();

  iterator end();

public: // ctors
  container() {};

private:
  std::array<slot, S> data;
  std::size_t size{0};
};

}; // namespace iuic
