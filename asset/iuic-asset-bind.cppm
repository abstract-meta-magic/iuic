// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:bind;
import std;
import iuic.underlying;

namespace iuic::asset {

struct binding_storage {

  template <typename T> bool exist() const {
    return data.contains(erasure::type::from<T>());
  };

  template <typename T> bool has_value() const {
    if (auto found = data.find(erasure::type::from<T>()); found != data.end()) {
      return found->second.get_type() != erasure::type::none();
    }
    return false;
  };

  template <typename T> T &get() {
    if (auto found = data.find(erasure::type::from<T>()); found != data.end()) {
      return *static_cast<T *>(found->second.raw());
    }
    throw std::logic_error{"Invalid get"};
  };

  // allocate slot
  template <typename T> void add() {
    if (not data.contains(erasure::type::from<T>())) {
      data.insert(erasure::type::from<T>(), nullptr);
    }
    throw std::logic_error{"Invalid add"};
  };

  // deallocate slot
  template <typename T> void remove() { data.erase(erasure::type::from<T>()); };

  // reset value
  template <typename T> void reset() {
    if (auto found = data.find(erasure::type::from<T>()); found != data.end()) {
      erasure::visited::as_garbage{found->second}.destruct();
      found->second = nullptr;
    }
  };

  // set value or replace old value
  template <typename T, typename... ARGS> void emplace(ARGS &&...args) {
    if (auto found = data.find(erasure::type::from<T>()); found != data.end()) {
      auto es = found->second;
      erasure::visited::as_garbage g{es};
      g.free();

      found->second = erasure::visited{new T{std::forward<ARGS>(args)...}};
    } else {
      data.insert(
          {erasure::type::from<T>(), new T{std::forward<ARGS>(args)...}});
    };
  };

  ~binding_storage() {
    for (auto &[key, value] : data) {
      if (value.get_type() != erasure::type::none()) {
        erasure::visited::as_garbage g{value};
        g.free();
      }
    }
  };

private:
  std::unordered_map<const erasure::type *, erasure::visited> data;
};

export template <typename T> struct bind {
  bind(binding_storage &storage_) : storage{std::addressof(storage_)} {};
  bind(binding_storage *storage_) : storage{storage_} {};

  bool has_value() const { return storage->has_value<T>(); };

  // replace old value
  template <typename... ARGS> void emplace(ARGS &&...args) {
    storage->emplace<T>(std::forward<ARGS>(args)...);
  };

  void reset() { storage->reset<T>(); };

  T &get() { return storage->get<T>(); };

  const T &get() const { return storage->get<T>(); };

private:
  binding_storage *storage;
};

}; // namespace iuic::asset
