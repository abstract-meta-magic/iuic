
module;

#include <any>
#include <concepts>
#include <type_traits>

export module iuic.core:storage;
import :base;

namespace iuic {
struct storage_type {};

// Дешевый ключь для быстрого поиска значения
// Должен мало весить
struct storage_registry_key {};

template <typename T>
concept pure_type = std::same_as<std::remove_cvref_t<T>, T> &&
                    not std::is_pointer_v<T> && not std::is_array_v<T>;

template <typename T> constexpr inline const storage_type *instance_stt() {
  static constexpr storage_type _{};
  return &_;
}

template <pure_type T> constexpr inline const storage_type *get_stt() {
  using clear_type = std::remove_cvref_t<T>;

  return instance_stt<clear_type>();
};

struct base_ref {
  template <typename T> constexpr bool as() const {
    return type == get_stt<T>();
  };

  template <pure_type T>
  base_ref(T *object) : data{object}, type{get_stt<T>()} {}

protected:
  void *data;
  const storage_type *type;
};
} // namespace iuic
export namespace iuic {

struct storage_ref final : base_ref {
  template <pure_type T> storage_ref(T *object) : base_ref{object} {}
  template <typename T> constexpr T &unwrap() const {
    if (data && as<T>()) {
      return *static_cast<T *>(data);
    }
    // TODO : normal exception
    throw "Inccorect type request";
  };
};

struct storage_cref final : base_ref {
  template <pure_type T> storage_cref(T *object) : base_ref{object} {}
  template <typename T> constexpr const T &unwrap() const {
    if (data && as<T>()) {
      return *static_cast<T *>(data);
    }
    // TODO : normal exception
    throw "Inccorect type request";
  };
};

struct storage_val {};
}; // namespace iuic

namespace iuic {

class storage {
public:
  template <typename Key> storage_ref get_ref(Key) { return {&empty}; };

  template <typename Key> storage_cref get_ref(Key) const { return {&empty}; };

  storage_ref get_ref(storage_registry_key) { return {&empty}; };

  storage_cref get_ref(storage_registry_key) const { return {&empty}; };

  template <typename Key> storage_registry_key get_key(Key) const {
    return {};
  };

  // Закрепление внешнего значения по текущему ключу
  template <typename Key, pure_type Val> storage_registry_key bind(Key, Val *) {
    return {};
  };

  // Создание в хранилище нового значения
  template <typename Key, pure_type Val>
  storage_registry_key emplace(Key, Val &&) {
    return {};
  };

  // Ограниченный буфер для временных значений
  template <typename Key, pure_type Val>
  storage_registry_key tmp(Key, Val &&) const;

  // Удалить элимент по ключу
  template <typename Key> void erase(Key){};

  // Отчистка временного буфера
  void clear_tmp();

private: // data
  static inline int empty{2};
};

// Что-то вроде view на значение в storage
// Обязанно выжыть между двумя make
}; // namespace iuic
