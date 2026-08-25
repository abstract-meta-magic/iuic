// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:domain;
import std;
import iuic.underlying;

export namespace iuic::environment {

struct domain {

  template <typename T> struct proxy {
    proxy(domain *owner_) : owner{owner_} {};

    bool has_value() const {
      auto it = owner->data.find(type);
      return it != owner->data.end();
    };

    // TODO : bool init_if_not(auto &&call);

    // TODO : template <auto &&call> bool init_if_not() {};

    T &get_ref() {
      if (auto it = owner->data.find(type); it != owner->data.end()) {
        return *static_cast<T *>(it->second.raw());
      }
      throw std::logic_error{"Domain type not exist"};
    };

    erasure::visited::as_mutable get() {
      if (auto it = owner->data.find(type); it != owner->data.end()) {
        return erasure::visited{it->second.raw(), it->first};
      } else {
        return nullptr;
      }
    };

    template <typename U = T, typename... ARGS>
      requires std::is_base_of_v<T, U>
    bool try_emplace(ARGS &&...args) {
      if (not has_value()) {
        U *ptr = new U{std::forward<ARGS>(args)...};
        owner->data.insert({type, ptr});
        return true;
      }
      return false;
    };

    const erasure::type *get_native_type() const {
      if (auto it = owner->data.find(type); it != owner->data.end()) {
        return it->second.get_type();
      } else {
        return erasure::type::none();
      };
    };

    template <typename U = T> bool try_set(U &&value) {
      if (not has_value()) {
        U *ptr = new U{std::forward<U>(value)};
        owner->data.insert({type, ptr});
        return true;
      }
      return false;
    };

  private:
    static constexpr const erasure::type *type{erasure::type::from<T>()};
    domain *owner;
  };

  template <typename T> proxy<T> get() { return proxy<T>{this}; };

  ~domain() {
    for (auto &&[_, obj] : data) {
      if (obj.get_type() != erasure::type::none()) {
        erasure::visited::as_garbage{obj}.free();
      }
    }
  };

private:
  std::unordered_map<const erasure::type *, erasure::visited> data;
};
}; // namespace iuic::environment
