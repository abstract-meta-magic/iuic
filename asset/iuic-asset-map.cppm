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

  struct basic_accessor {
    bool has_value() const {
      if (is_found) {
        return owner->data.at(index).value.has_value();
      }
      return false;
    };

    basic_accessor(Key key_, container *owner_)
        : key{key_}, owner{owner_}, index{0}, is_found{false} {
      find_();
    }

    basic_accessor(const basic_accessor &) = delete ("proxy struct");
    basic_accessor(basic_accessor &&) = delete ("proxy struct");
    basic_accessor &operator=(const basic_accessor &) = delete ("proxy struct");
    basic_accessor &operator=(basic_accessor &&) = delete ("proxy struct");

  private:
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

  protected:
    static constexpr auto mask = S - 1;
    Key key;
    container *owner;
    std::size_t index;
    bool is_found;
  };

  struct const_accessor : basic_accessor {
    const_accessor(Key key_, container *owner_)
        : basic_accessor{key_, owner_} {};

    Value &get() const {
      // in to assert ?
      if (this->is_found) {
        return this->owner->data.at(this->index).value.value();
      }
      throw std::logic_error{"Access to unset variable"};
    };
  };

  struct accessor : basic_accessor {
    accessor(Key key_, container *owner_) : basic_accessor{key_, owner_} {};

    Value &get() {
      // in to assert ?
      if (this->is_found) {
        return this->owner->data.at(this->index).value.value();
      }
      throw std::logic_error{"Access to unset variable"};
    };

    // invalidate accessor
    template <typename T> bool set(T &&value) && {
      if (this->is_found) {
        this->owner->data.at(this->index).value.emplace(std::forward<T>(value));
        return true;
      } else {
        std::size_t index_ = this->index;
        for (std::size_t i{0}; i < 10; ++i) {
          if (auto &slot = this->owner->data.at(index_);
              not slot.value.has_value()) {
            slot.key = this->key;
            slot.value.emplace(std::forward<T>(value));
            this->is_found = true;
            this->index = index_;
            ++this->owner->size_;
            return true;
          }

          index_ = (index_ + 1) & this->mask;
        };
      }
      return false;
    };

    // invalidate accessor
    void reset() && {
      if (this->is_found) {
        auto &slot = this->owner->data.at(this->index);

        slot.key = {};
        slot.value.reset();
        --this->owner->size_;
        this->is_found = false;
      }
    };
  };

  struct iterator {
    // value type is proxy
    // ++
    // --
    // ==
    iterator(container *owner_, std::size_t index_)
        : owner{owner_}, index{index_} {}

    bool valid() const { return index != S && owner->data[index].has_value(); };

    bool operator==(const iterator &other) const {
      return index == other.index;
    };

    Value &operator*() { return owner->data[index].value.value(); };

    iterator &operator++() {
      ++index;
      return +*this;
    };

    iterator &operator+() {
      for (; index < S; ++index) {
        if (owner->data[index].value.has_value()) {
          break;
        }
      }
      return *this;
    };

  private:
    container *owner;
    std::size_t index;
  };

  accessor access(Key key) { return accessor{key, this}; };
  const_accessor access(Key key) const { return const_accessor{key, this}; }

  iterator begin() {
    if (empty()) {
      return iterator{this, S};
    } else {
      return +iterator{this, 0};
    };
  };

  iterator end() { return iterator{this, S}; };

  iterator begin() const {
    if (empty()) {
      return iterator{this, S};
    } else {
      return +iterator{this, 0};
    };
  };

  iterator end() const { return iterator{this, S}; };

  std::size_t size() const { return size_; };

  bool empty() const { return size_ == 0; };

public: // ctors
  container() {};

private:
  std::array<slot, S> data;
  std::size_t size_{0};
};

}; // namespace iuic
