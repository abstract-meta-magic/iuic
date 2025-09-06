
module;

#include <map>
#include <print>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:event;
import :base;
import :fct;
import :storage;
export import :key_code;

export namespace iuic {

// rename and make this interface
struct element_state_selector {

  bool is_hovered(uid_t uid) { return false; };

  bool is_active(uid_t uid) { return false; };

  bool is_focused(uid_t uid) { return false; };

  void set_active(uid_t uid) {};

  void set_focused(uid_t uid) {};

  void set_hovered(uid_t uid) {};

  void unset_focused(uid_t uid) {};

  void unset_active(uid_t uid) {};

  void unset_focused() {};

  void unset_active() {};
};

// make this constexpr ???
struct pseudo_selector {
  virtual ~pseudo_selector() = default;

  virtual bool is_hovered(uid_t uid) const noexcept = 0;

  virtual bool is_active(uid_t uid) const noexcept = 0;

  virtual bool is_focused(uid_t uid) const noexcept = 0;

  virtual void set_active(uid_t uid) noexcept = 0;

  virtual void set_focused(uid_t uid) noexcept = 0;

  virtual void set_hovered(uid_t uid) noexcept = 0;

  virtual void unset_focused(uid_t uid) noexcept = 0;

  virtual void unset_active(uid_t uid) noexcept = 0;

  virtual void unset_focused() noexcept = 0;

  virtual void unset_active() noexcept = 0;

  virtual void unset_hovered(uid_t uid) noexcept = 0;
};

struct event_type {
  struct event_base {
    storage &storage;
    pseudo_selector &selector;
    storage_registry_key srk;
  };

  struct pointer_enter : public event_base {
    uid_t uid;
  };

  struct pointer_exit : public event_base {
    uid_t uid;
  };

  struct pointer_move : public event_base {
    uid_t uid;
  };

  struct key : public event_base {
    key_code code;
  };

  struct key_u : public key {
    uid_t uid;
  };

  // global
  struct key_g : public key_u {};

  // hovered
  struct key_h : public key_u {};

  // focused
  struct key_f : public key_u {};

  // active
  struct key_a : public key_u {};
};
}; // namespace iuic
namespace iuic {

// pointer
using pointer_enter_event_fpt = void (*)(event_type::pointer_enter);
using pointer_exit_event_fpt = void (*)(event_type::pointer_exit);
using pointer_move_event_fpt = void (*)(event_type::pointer_move);

// key
using key_global_event_fpt = void (*)(event_type::key_g);
using key_hovered_event_fpt = void (*)(event_type::key_h);
using key_focused_event_fpt = void (*)(event_type::key_f);
using key_active_event_fpt = void (*)(event_type::key_a);

using variadic_key_callback =
    std::variant<key_focused_event_fpt, key_global_event_fpt,
                 key_hovered_event_fpt, key_active_event_fpt>;

using variadic_pointer_callback =
    std::variant<pointer_enter_event_fpt, pointer_exit_event_fpt,
                 pointer_move_event_fpt>;

template <typename T>
concept key_event_callback_cpt =
    requires(T &&call) { variadic_key_callback{call}; };

template <typename T>
concept pointer_event_callback_cpt =
    requires(T &&call) { variadic_pointer_callback{call}; };

// хешировать
struct key_event {
  storage_registry_key data;
  variadic_key_callback call;
  size_t id;
  uid_t uid;
};

struct pointer_event {
  storage_registry_key data;
  variadic_pointer_callback call;
  size_t id;
  uid_t uid;
};

// Нужно проработать интерфейст
// для удобства внутреннего использования
// Нужно лучше продумать роль этого класса
// в системе событий
struct event_pack {
  using hit_map_t = std::map<ui_rect, std::vector<pointer_event>>;
  using key_event_map = std::map<uid_t, std::vector<key_event>>;

  key_event_map key;
  hit_map_t pointer;
};

// главная обязанность - сборка событий
class event_collector {
public:
  event_collector() {}

  void reset() {};

  void push(const key_event &e) { events.push_back({e}); };

  void push(key_event &&e) { events.push_back({e}); };

  void push(const pointer_event &e) { events.push_back({e}); };

  void push(pointer_event &&e) { events.push_back({e}); };

  event_pack build_pack(const FCTree &ctree) {
    // Bag тут не пропускаются discarded элементы
    event_pack res;

    for (auto &var : events) {
      std::visit(
          [&](auto &obj) {
            if constexpr (std::same_as<std::remove_cvref_t<decltype(obj)>,
                                       key_event>) { // do key
              if (not ctree.at(obj.id).is_discarted()) {
                res.key[obj.uid].push_back(obj);
              }
            } else { // do pointer
              if (not ctree.at(obj.id).is_discarted()) {
                auto &celement = ctree.at(obj.id);
                res.pointer[celement.get_rect()].push_back(obj);
              }
            };
          },
          var);
    };

    events.clear();

    return res;
  };

private:
  // В теории можно сделать 2\3 буффиризацию
  // промежуточное решение
  std::vector<std::variant<key_event, pointer_event>> events;
};

// позже будет разработан uuid
// он будет использоватся для сохранения
// состояния между кадрами
// ОСНОВНАЯ ОБЯЗАННОСТЬ :
// корректная подготовка и отправка событий
class event_reciver final : private pseudo_selector {
  friend void apply_event_hit_surface(event_reciver &, event_pack &&);

public:
  void key(key_code key) {
    // TOTO : storage access
    storage st;

    for (auto &&e : event_pack.key[0]) {
      std::visit(
          [&, this](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              iuic::event_type::key_g>) {
              call(iuic::event_type::key_g{st, *this, e.data, key, 0});
            }
          },
          e.call);
    }

    if (hovered != 0) {
      for (auto &&e : event_pack.key[hovered]) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::key_h>) {
                call(iuic::event_type::key_h{st, *this, e.data, key, hovered});
              }
            },
            e.call);
      }
    }

    if (focused != 0) {

      for (auto &&e : event_pack.key[focused]) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::key_f>) {
                call(iuic::event_type::key_f{st, *this, e.data, key, focused});
              }
            },
            e.call);
      }
    }

    if (active != 0) {
      for (auto &&e : event_pack.key[focused]) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::key_a>) {
                call(iuic::event_type::key_a{st, *this, e.data, key, active});
              }
            },
            e.call);
      }
    }
  };

  void pointer(ui_position position) {
    storage st;
    for (auto &&[_, obj] : event_pack.pointer) {
      for (auto &&e : obj) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::pointer_move>) {
                call(event_type::pointer_move{st, *this, e.data, e.uid});
              }
            },
            e.call);
      };
    }
  };

  // set position without events
  void pointer_set(ui_position position) {};

  bool in__(const ui_rect &rect, const ui_position &pos) {
    return (
        pos.x >= rect.position.x && pos.x < (rect.position.x + rect.size.w) &&
        pos.y >= rect.position.y && pos.y < (rect.position.y + rect.size.h));
  };

  void on_enter__(std::vector<pointer_event> &events) {
    storage st;
    for (auto &e : events) {
      std::visit(
          [&](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              event_type::pointer_enter>) {
              std::println("event {}", e.uid);
              if (e.uid != hovered) {
                std::println("event go ");
                call(event_type::pointer_enter{st, *this, e.data, e.uid});
              }
            }
          },
          e.call);
    }
  };
  void on_exit__(std::vector<pointer_event> &events) {
    storage st;
    for (auto &e : events) {
      std::visit(
          [&](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              event_type::pointer_exit>) {
              std::println("event {}", e.uid);
              if (e.uid == hovered) {
                std::println("event go ");
                call(event_type::pointer_exit{st, *this, e.data, e.uid});
              }
            }
          },
          e.call);
    }
  };

  // just move the pointer
  void pointer_move(ui_position position) {

    for (auto &&[rect, events] : event_pack.pointer) {
      if (in__(rect, pointer_position)) {
        if (not in__(rect, position)) {
          on_exit__(events);
        }
      } else {
        if (in__(rect, position)) {
          std::println("on enter");
          on_enter__(events);
        }
      };
    }

    // std::println("pointer pos {}:{}", position.x, position.y);

    pointer_position = position;
    // on_move after calc hovered_element

    // chech ui_rect and pointer intersept
    // найти все пересеченные ui_rect
    // мнежду двуня position
  };

  // in version 0.2
  void key_buff(key_code);

  // in version 0.2
  void key_buff_dispatch(key_code);

  event_reciver() {};

private: // pseudo-selector impl
  bool is_hovered(uid_t uid) const noexcept override { return hovered == uid; };

  bool is_active(uid_t uid) const noexcept override { return active == uid; };

  bool is_focused(uid_t uid) const noexcept override { return focused == uid; };

  void set_active(uid_t uid) noexcept override { active = uid; };

  void set_focused(uid_t uid) noexcept override { focused = uid; };

  void set_hovered(uid_t uid) noexcept override { hovered = uid; };

  // TODO : future in set<uid_t>
  void unset_focused(uid_t uid) noexcept override { focused = 0; };

  void unset_hovered(uid_t uid) noexcept override { hovered = 0; };

  // TODO : future in set<uid_t>
  void unset_active(uid_t uid) noexcept override { active = 0; };

  void unset_focused() noexcept override { focused = 0; };

  void unset_active() noexcept override { active = 0; };

private:
  ui_position pointer_position;

  uid_t hovered{0}; // stack
  uid_t focused{0}; // set
  uid_t active{0};  // set
  event_pack event_pack{};
};

void apply_event_hit_surface(event_reciver &er, event_pack &&ep) {
  std::swap(er.event_pack, ep);
}

}; // namespace iuic
