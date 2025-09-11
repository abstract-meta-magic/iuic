
module;

#include <map>
#include <print>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

export module iuic.core:event;
import :base;
import :fct;
import :storage;
import :pseudo_selector;
export import :key_code;

export namespace iuic {

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
class event_reciver final {
  friend void apply_event_hit_surface(event_reciver &, event_pack &&);

public:
  void key(key_code key) {
    // TOTO : storage access

    for (auto &&e : event_pack.key[0]) {
      std::visit(
          [&, this](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              iuic::event_type::key_g>) {
              call(iuic::event_type::key_g{storage, selector, e.data, key, 0});
            }
          },
          e.call);
    }

    for (auto &&uid : selector_snapshot.hovered) {
      for (auto &&e : event_pack.key[uid]) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::key_h>) {
                call(iuic::event_type::key_h{storage, selector, e.data, key,
                                             uid});
              }
            },
            e.call);
      }
    }

    for (auto &&uid : selector_snapshot.focused) {
      for (auto &&e : event_pack.key[uid]) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::key_f>) {
                call(iuic::event_type::key_f{storage, selector, e.data, key,
                                             uid});
              }
            },
            e.call);
      }
    }

    for (auto &&uid : selector_snapshot.active) {
      for (auto &&e : event_pack.key[uid]) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::key_a>) {
                call(iuic::event_type::key_a{storage, selector, e.data, key,
                                             uid});
              }
            },
            e.call);
      }
    }
  };

  void pointer(ui_position position) {
    for (auto &&[_, obj] : event_pack.pointer) {
      for (auto &&e : obj) {
        std::visit(
            [&](auto &call) {
              if constexpr (std::is_invocable_v<
                                std::remove_cvref_t<decltype(call)>,
                                iuic::event_type::pointer_move>) {
                call(
                    event_type::pointer_move{storage, selector, e.data, e.uid});
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
    for (auto &e : events) {
      std::visit(
          [&](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              event_type::pointer_enter>) {
              std::println("event {}", e.uid);
              if (not selector_snapshot.hovered.contains(e.uid)) {
                std::println("event go ");
                call(event_type::pointer_enter{storage, selector, e.data,
                                               e.uid});
              }
            }
          },
          e.call);
    }
  };
  void on_exit__(std::vector<pointer_event> &events) {
    for (auto &e : events) {
      std::visit(
          [&](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              event_type::pointer_exit>) {
              std::println("event {}", e.uid);
              if (selector_snapshot.hovered.contains(e.uid)) {
                std::println("event go ");
                call(
                    event_type::pointer_exit{storage, selector, e.data, e.uid});
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

  event_reciver(storage &storage_, advanced_pseudo_selector &selector_)
      : storage{storage_}, selector{selector_} {};

private:
  storage &storage;
  advanced_pseudo_selector &selector;
  pseudo_selector_snapshot selector_snapshot;
  ui_position pointer_position;

  event_pack event_pack{};
};

void apply_event_hit_surface(event_reciver &er, event_pack &&ep) {
  std::swap(er.event_pack, ep);
  er.selector_snapshot = er.selector.snapshot();
}

}; // namespace iuic
