// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.utils:external;
import std;
import :decl;

// TODO : need full REWORK
// Чего я хочу
// runtime\ct дискрипторы
// для возможности ссылаться на ассеты или ресурсы
namespace iuic::external {
namespace type {
export using decl = utils::ctype<utils::anonim_tag()>;

export struct value {
  constexpr auto operator<=>(const value &) const = default;
  const decl *decl{nullptr};

  constexpr value(const type::decl *v) : decl{v} {};
  constexpr value(const type::decl &v) : value{std::addressof(v)} {};
};
}; // namespace type

export struct uri {
  constexpr uri(std::string_view str) {
    str.find("://");

    // + проверить на разрешенные символы
  }

  constexpr uri(std::string_view scheme, std::string_view path) {}

  constexpr std::string_view get_scheme() const { return {data, prefix_end}; };

  constexpr std::string_view get_path() const {
    return {data + prefix_end, path_size};
  };

private:
  char *data{nullptr};
  std::uint16_t size{0};
  std::uint16_t prefix_end{0}; // name + ://
  std::uint16_t path_size{0};
};

namespace V2 {
template <typename T> struct hidden_identity__ {
  using type = T;
};

export struct resolution_context {
  //
  constexpr const uri &get_uri() const;
};

struct native_instance {
  type::value type;
  uri path;
  erasure::visited declaration{nullptr};
};

struct block {
  constexpr virtual ~block() {};

  constexpr virtual bool try_acquire() = 0;

  constexpr virtual void retire() = 0;

  constexpr virtual void die() = 0;

  constexpr virtual resolution_context
  get_resolution_context_for(const native_instance *) = 0;
};

struct static_block__ : block {
  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retire() override {};

  constexpr virtual void die() override {};

  constexpr virtual resolution_context
  get_resolution_context_for(const native_instance *) override {
    return {};
  };
};

struct dynamic_block__ : block {
  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retire() override {};

  constexpr virtual void die() override {};

  constexpr virtual resolution_context
  get_resolution_context_for(const native_instance *) override {
    return {};
  };
};

struct instance : private native_instance {
  constexpr bool is_resolved() const { return false; };
  bool resolve(auto ctx) const { return false; }; // use 'utils'{block} ???
  void reset() const {};
  constexpr type::value get_binding_type() const {
    return nullptr;
  }; // -> binding::type::decl
  constexpr const erasure::type *get_declaration_type() const {
    return declaration.get_type();
  }; // -> erasure::type
  constexpr const erasure::type *get_resolution_type() const {
    return nullptr;
  }; // -> erasure::type
  constexpr bool try_visit_declaration(auto &&call) const {
    return erasure::visited::as_const{declaration}.try_visit(
        std::forward<decltype(call)>(call));
  };
  constexpr bool try_visit_resolution(auto) const { return false; };

public: // ctor
  constexpr instance(type::value type_, uri path_, block *block_ptr_)
      : native_instance{type_, path_}, block_ptr{block_ptr_} {}

  template <typename T, typename UNIQUE = decltype([]() {})>
  constexpr instance(type::value type_, uri path_, block *block_ptr_, T &&obj)
      : native_instance{type_, path_, obj}, block_ptr{block_ptr_} {}

public: // BIG-V + RAII
  constexpr ~instance() {
    if not consteval {
      if (declaration.get_type() != nullptr) {
        erasure::visited::as_garbage{declaration}.destruct();
        erasure::visited::as_garbage{declaration}.free();
      }
    }
  };

private:
  block *block_ptr;
};

template <typename T>
constinit static_block__ static_block{

};

export struct binding {

  template <typename T = decltype([]() {})>
  constexpr binding(type::value type_, uri path_, hidden_identity__<T> = {})
      : type{type_}, path{path_}, block_ptr{nullptr} {
    if consteval {
      block_ptr = &static_block<T>;
    } else {
      block_ptr = new dynamic_block__{};
    }
  };

  // make conceptr is_data_type
  template <auto obj> constexpr instance make_declaration() const {
    return {type, path, block_ptr, obj};
  };

  template <typename T> instance make_declaration(T &&obj) const {
    return instance{type, path, block_ptr, obj};
  };

  constexpr instance make_declaration() const {
    return {type, path, block_ptr};
  };

public: // BIG-V + RAII
  binding(const binding &) = delete;
  binding(binding &&) = delete;
  // can
  binding &operator=(const binding &) = delete;
  binding &operator=(binding &&) = delete;
  constexpr ~binding() {
    if not consteval {
      if (block_ptr) {
        block_ptr->die();
      }
    }
  };

private:
  type::value type;
  uri path;
  block *block_ptr{nullptr};
};

template <const type::decl &> struct strict_binding {};

}; // namespace V2

export struct binding {
  virtual ~binding() {};
  virtual type::value type() = 0;
};

namespace capabilities {

export struct decl {
  constexpr decl() = default;
  decl(const decl &) = delete;
  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  constexpr bool operator==(const decl &other) const {
    return this == std::addressof(other);
  }
};

export struct ref {
  constexpr ref(const decl &d) : value{std::addressof(d)} {};
  constexpr ref(const decl *d) : value{d} {};

  constexpr bool operator==(const ref &other) const {
    return value == other.value;
  }
  const decl *value{};
};

}; // namespace capabilities
}; // namespace iuic::external

/*
constexpr iuic::external::type::decl my_type;

void heh() {
  constexpr iuic::external::V2::binding bd{
      my_type, iuic::external::uri{"res://data/hah"}};

  struct my_data {
    int x, y;
  };

  struct sus {
    iuic::external::V2::instance inst;
  };

  struct backend_data {};

  constexpr auto inst = bd.make_declaration<my_data{4, 4}>();

  constexpr auto ss = [&]() {
    return sus{bd.make_declaration<my_data{5, 7}>()};
  }();

  auto decl = bd.make_declaration(int{7});

  constexpr bool heh =
      ss.inst.get_declaration_type() == iuic::erasure::type::from<my_data>();
  // всегда declared

  bool aoeu = ss.inst.try_visit_declaration([](const my_data &value) {});

  inst.is_resolved();
  // resolution_environment
  // or
  // resolution_context
  inst.resolve([&](iuic::external::V2::resolution_context ctx) {
    if (auto &uri = ctx.get_uri(); uri.get_scheme() == "resource://") {
      // do job
    }
  });

  bool hueeh =
      inst.get_declaration_type() == iuic::erasure::type::from<my_data>();

  inst.reset();

  inst.get_binding_type();     // -> binding::type::decl
  inst.get_declaration_type(); // -> erasure::type
  inst.get_resolution_type();  // -> erasure::type
  inst.try_visit_declaration([](const my_data &) {});
  inst.try_visit_resolution([](backend_data &) {});
};

void test_2(iuic::external::V2::instance &ints) {
  if (ints.get_binding_type() == my_type) {
    if (ints.is_resolved()) {
      ints.try_visit_resolution([]() {
        // do job
      });
    }
  }
}
*/
