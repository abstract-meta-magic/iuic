// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.base;
import std;
import iuic.underlying.erasure;
import iuic.underlying.utils;
import :type;
import :uri;

namespace iuic::external {

struct declaration_segment {
  type::value type;
  uri path;
  erasure::visited object{nullptr};
};

struct resolution_segment {
  erasure::visited object{nullptr};
  // callback's
};

// контескт на который ссылаются все сегменты
// resolution_segment -> general_context
// instance           -> binding
struct general_context {
  template <typename T> T *allocate() {
    auto *ptr = new T;

    allocated.push_back(ptr);

    return ptr;
  };

  void dellocate(const void *ptr) {
    // DO DEALLOCATE
  };

  template <typename T> T *get_shared() {
    static constexpr auto *type = erasure::type::from<T>();

    for (auto &object : shared) {
      if (object.get_type() == type) {
        return static_cast<T *>(object.raw());
      };
    }

    return nullptr;
  };

  template <typename T> T *allocate_shared() {
    auto *ptr = new T;

    shared.push_back(ptr);

    return ptr;
  };

  template <typename T>
  void delocate_shared() {
    // dector + delete
    // DO DEALLOCATE
  };

  ~general_context() {
    for (auto &object : allocated) {
      if (object.get_type() != erasure::type::none()) {
        erasure::visited::as_garbage{object}.free();
      }
    }

    for (auto &object : shared) {
      if (object.get_type() != erasure::type::none()) {
        erasure::visited::as_garbage{object}.free();
      }
    }
  };

  std::vector<erasure::visited> allocated;

  std::vector<erasure::visited> shared;
};

struct resolution_segment_proxy {
  resolution_segment_proxy(resolution_segment *resolution_,
                           general_context *general_)
      : resolution{resolution_}, general{general_} {};

  void reset() {
    erasure::visited::as_garbage{resolution->object}.destruct();

    general->dellocate(resolution->object.raw());
  };

  bool has_value() const {
    return resolution->object.get_type() != erasure::type::none();
  };

  erasure::visited::as_mutable get() { return resolution->object; };

  operator bool() const { return has_value(); }

  template <typename T> bool try_emplace(auto &&...args) {
    if (has_value()) {
      return false;
    }

    T *mem = general->allocate<T>();

    new (mem) T{std::forward<decltype(args)>(args)...};

    resolution->object = mem;

    return true;
  };

  bool try_set(auto &&value) {
    using type = std::remove_cvref_t<decltype(value)>;

    if (has_value()) {
      return false;
    }

    type *mem = general->allocate<type>();

    new (mem) type{std::forward<decltype(value)>(value)};

    resolution->object = mem;

    return true;
  };

private:
  resolution_segment *resolution;
  general_context *general;
};

template <typename T> struct shared_object_proxy {

  shared_object_proxy(general_context *general_) : general{general_} {}

  void reset() { general->delocate_shared<T>(); };

  bool has_value() const { return general->get_shared<T>(); };

  operator bool() const { return has_value(); }

  bool try_emplace(auto &&...args) {
    if (has_value()) {
      return false;
    }

    T *mem = general->allocate_shared<T>();

    new (mem) T{std::forward<decltype(args)>(args)...};

    return true;
  };

  bool try_set(auto &&value) {
    if (has_value()) {
      return false;
    }

    T *mem = general->allocate_shared<T>();

    new (mem) T{std::forward<decltype(value)>(value)};

    return true;
  };

  erasure::visited::as_mutable get() { return general->get_shared<T>(); };

private:
  general_context *general;
};

// rt
export struct resolution_context {

  resolution_segment_proxy get_resolution() { return {resolution, general}; };

  template <typename T> shared_object_proxy<T> get_shared_object() {
    return {general};
  };

  const uri &get_uri() const { return declaration->path; };

  const type::value get_binding_type() const { return declaration->type; };

  const erasure::type *get_declaration_type() const {
    return declaration->object.get_type();
  };

  bool try_visit_declaration(auto &&call) const {
    return erasure::visited::as_const{declaration->object}.try_visit(
        std::forward<decltype(call)>(call));
  };

  resolution_context(const declaration_segment *declaration_,
                     resolution_segment *resolution_, general_context *general_)
      : declaration{declaration_}, resolution{resolution_}, general{general_} {}

private:
  const declaration_segment *declaration;
  resolution_segment *resolution;
  general_context *general;
};

struct block {
  constexpr virtual ~block() {};

  constexpr virtual bool try_acquire() = 0;

  constexpr virtual void retire() = 0;

  constexpr virtual void die() = 0;

  constexpr virtual resolution_segment *
  get_resolution_segment_for(const declaration_segment *) = 0;

  constexpr virtual general_context *get_general_context() = 0;
};
}; // namespace iuic::external
