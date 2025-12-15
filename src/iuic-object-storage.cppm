
module;

export module iuic.core:storage.object;
import std;
import :base;
import :hash;
import :storage.def;

namespace iuic {

struct storage_type {
  using dector_t = void (*)(void *);
  std::size_t size;
  std::size_t align;
  dector_t dector;
};

// Дешевый ключь для быстрого поиска значения
// Должен мало весить

template <typename T>
concept pure_type = std::same_as<std::remove_cvref_t<T>, T> &&
                    not std::is_pointer_v<T> && not std::is_array_v<T>;

template <typename T>
constexpr inline const storage_type *instance_storage_type__() {
  static constexpr storage_type _{
      sizeof(T), alignof(T), [](void *obj) { static_cast<T *>(obj)->~T(); }};
  return &_;
}

// TODO : normal naming
template <pure_type T> constexpr inline const storage_type *storage_type_of() {
  using clear_type = std::remove_cvref_t<T>;

  return instance_storage_type__<clear_type>();
};

struct base_ref {
  void *data{nullptr};
  const storage_type *type{storage_type_of<std::nullptr_t>()};
};
} // namespace iuic

namespace iuic {

// декомпазировать на use и set части
class object_storage {
private: // visit help
  template <typename T> static consteval void visit_arg_type__(T);

  template <typename T, typename ARG>
  static consteval ARG visit_arg_type__(std::function<T(ARG &)>);

  template <typename T, typename ARG>
  static consteval ARG visit_traits__(std::function<T(const ARG &)>);

public:
  bool exist(ork_t srk) const { return is_init__(srk); };

  template <pure_type T> bool exist_as(ork_t srk) const {
    if (exist(srk)) {
      // return storage_ref{get_ref__(srk)}.as<T>();
    }
    return false;
  };

  constexpr bool try_visit(ork_t srk, auto &&call) {
    auto ref = get_ref__(srk);

    using type =
        std::remove_cvref_t<decltype(visit_arg_type__(std::function{call}))>;

    static_assert(not std::same_as<type, void>,
                  "Invalide visit type. Please use void (*)(T&) for callback.");

    if (ref.type == storage_type_of<type>()) {
      return call(*static_cast<type *>(ref.data)), true;
    }

    return false;
  };

  constexpr bool visit_or(ork_t ork, auto &&call,
                          std::invocable<> auto &&call_or) {
    if (try_visit(ork, std::forward<decltype(call)>(call))) {
      return true;
    } else {
      return call_or(), false;
    };
  }

  constexpr bool visit_switch(ork_t ork, auto &&...calls) noexcept {
    auto ref = get_ref__(ork);

    return ((ref.type ==
                 storage_type_of<std::remove_cvref_t<decltype(visit_arg_type__(
                     std::function{calls}))>>()
             ? calls(*static_cast<std::remove_cvref_t<decltype(visit_arg_type__(
                         std::function{calls}))> *>(ref.data)),
             true : false) ||
            ...);
  }

  bool try_visit(ork_t srk, auto &&call) const {
    // storage_cref ref{get_ref__(srk)};
    base_ref ref{};

    using type =
        std::remove_cvref_t<decltype(visit_arg_type__(std::function{call}))>;

    static_assert(
        not std::same_as<type, void>,
        "Invalide visit type. Please use void(const T&) for callback.");

    if (ref.type == storage_type_of<type>()) {
      return call(*static_cast<const type *>(ref.data)), true;
    }

    return false;
  };

protected:
  constexpr base_ref get_ref__(ork_t srk) {
    switch (persistent_geniration_phase) {
    case persistent_geniration_phase_e::phase__1: {
      if (persistent__1.contains(srk)) {
        return persistent__1[srk];
      }
      break;
    };
    case persistent_geniration_phase_e::phase__2: {
      if (persistent__2.contains(srk)) {
        return persistent__2[srk];
      }
      break;
    }
    }

    if (tmp__.contains(srk)) {
      return tmp__[srk];
    }

    // null
    return base_ref{};
  };

  static constexpr hash::hash_t srk_hash_seed{445736667};

  // magic number for tmp + uid + name
  static ork_t make_tmp_srk__(uid_t uid, const std::string &name) noexcept {
    static constexpr char tmp_magick{'T'};

    std::stringstream ss;
    ss << tmp_magick << uid;

    if (name.empty()) {
      ss << "__unnamed__";
    } else {
      ss << name;
    }

    auto str = ss.str();

    return hash::make(srk_hash_seed, str.c_str(), str.length());
  };

  // magic number for persist + uid + name
  static ork_t make_persist_srk__(uid_t uid, const std::string &name) noexcept {
    static constexpr char persistent_magick{'P'};

    std::stringstream ss;
    ss << persistent_magick << uid;

    if (name.empty()) {
      ss << "__unnamed__";
    } else {
      ss << name;
    }

    auto str = ss.str();

    return hash::make(srk_hash_seed, str.c_str(), str.length());
    return 2;
  };

protected: // data
  bool is_init__(ork_t srk) const { return false; };

  struct raw_memory {
    std::byte *begin;
    std::size_t size;
  };

  // tmp managment
  std::map<ork_t, base_ref> tmp__;

  // persistent managment
  enum class persistent_geniration_phase_e {
    phase__1,
    phase__2
  } persistent_geniration_phase{persistent_geniration_phase_e::phase__1};

  std::map<ork_t, base_ref> persistent__1;
  std::map<ork_t, base_ref> persistent__2;
  std::map<uid_t, ork_t> reg;
  // buff's
  raw_memory warning_persisten_memory__; // buff
  raw_memory base_persisten_memory__;    // buff
  raw_memory tmp_memory__;               // buff

  // upsteam is null_memory_resource
  std::pmr::monotonic_buffer_resource persistent_memory_warning__;
  // upsteam is persisten_memory_warning__
  std::pmr::monotonic_buffer_resource persistent_memory__;
  // upsteam is persisten_memory__
  std::pmr::unsynchronized_pool_resource persistent_resource;

  std::pmr::monotonic_buffer_resource tmp_resource;
};

struct mutable_object_storage : public object_storage {
  template <typename T> void set(ork_t, T &&);

  template <typename T>
  std::tuple<T &, object_registry_key> tmp(uid_t uid, const std::string &name,
                                           T &&object)
    requires std::is_trivially_destructible_v<T>
  {
    using pure_type = std::remove_cvref_t<T>;
    auto srk = make_tmp_srk__(uid, name);

    tmp__.try_emplace(srk, base_ref{});

    auto &ref = tmp__[srk];

    // force replace
    ref.type = storage_type_of<pure_type>();
    ref.data = tmp_resource.allocate(ref.type->size, ref.type->align);
    new (ref.data) pure_type{std::forward<T>(object)};

    return {*static_cast<T *>(ref.data), srk};
  };

  object_registry_key persist(uid_t uid, const std::string &name) {
    auto srk = make_persist_srk__(uid, name);

    switch (persistent_geniration_phase) {
    case persistent_geniration_phase_e::phase__1: {
      if (persistent__2.contains(srk)) {
        auto swap_value = persistent__2.extract(srk);
        persistent__1.insert(std::move(swap_value));
      } else {
        persistent__1.try_emplace(srk, base_ref{});
      };
      break;
    }
    case persistent_geniration_phase_e::phase__2: {
      if (persistent__1.contains(srk)) {
        auto swap_value = persistent__1.extract(srk);
        persistent__2.insert(std::move(swap_value));
      } else {
        persistent__2.try_emplace(srk, base_ref{});
      }
      break;
    }
    }

    return srk;
  };

  void init_if_not(ork_t srk, std::invocable<> auto &&call) {
    using pure_type = std::remove_cvref_t<std::invoke_result_t<decltype(call)>>;

    switch (persistent_geniration_phase) {
    case persistent_geniration_phase_e::phase__1: {
      if (persistent__1.contains(srk)) {
        auto &ref = persistent__1[srk];
        if (ref.type == storage_type_of<std::nullptr_t>()) {
          ref.type = storage_type_of<pure_type>();
          ref.data =
              persistent_resource.allocate(ref.type->size, ref.type->align);
          new (ref.data) pure_type{call()};

          std::println("INIT OBJECT - srk {}", srk.value);
        }
        return;
      }
      break;
    }
    case persistent_geniration_phase_e::phase__2: {

      if (persistent__2.contains(srk)) {
        auto &ref = persistent__2[srk];
        if (ref.type == storage_type_of<std::nullptr_t>()) {
          ref.type = storage_type_of<pure_type>();
          ref.data =
              persistent_resource.allocate(ref.type->size, ref.type->align);
          new (ref.data) pure_type{call()};

          std::println("INIT OBJECT - srk {}", srk.value);
        }
        return;
      }
      break;
    }
    }
  };
};
// module private класс используемый в context
// для упровления хранилищем
struct managed_object_storage final : public mutable_object_storage {
  // удаление временных значений и проверка персистентных
  void advance_generation() {

    switch (persistent_geniration_phase) {
    case persistent_geniration_phase_e::phase__1: {
      for (auto &&[_, ref] : persistent__2) {
        ref.type->dector(ref.data);
        persistent_resource.deallocate(ref.data, ref.type->size,
                                       ref.type->align);
      };
      persistent__2.clear();
      persistent_geniration_phase = persistent_geniration_phase_e::phase__2;
      break;
    };

    case persistent_geniration_phase_e::phase__2: {
      for (auto &&[_, ref] : persistent__1) {
        ref.type->dector(ref.data);
        persistent_resource.deallocate(ref.data, ref.type->size,
                                       ref.type->align);
      };

      persistent__1.clear();
      persistent_geniration_phase = persistent_geniration_phase_e::phase__1;
      break;
    };
    }

    for (auto &&[_, ref] : tmp__) {
      ref.type->dector(ref.data);
    };

    tmp_resource.release();
  };

  // удаление всех выделенных объектов
  void clear() {};

  // Медоды упровления буферами и другими опциями хранилища

  // tmp memory limit

  // persistent memory limit

  // max objects
  // max object size
};

// Что-то вроде view на значение в storage
// Обязанно выжыть между двумя make
}; // namespace iuic
