
module;

#include <unordered_map>
#include <unordered_set>

export module iuic.core:state;
import :base;

namespace iuic {
struct pseudo_state_decl final {
  consteval pseudo_state_decl() noexcept {};

  pseudo_state_decl(const pseudo_state_decl &) = delete;
  pseudo_state_decl &operator=(const pseudo_state_decl &) = delete;
  pseudo_state_decl(pseudo_state_decl &&) = delete;
  pseudo_state_decl &operator=(pseudo_state_decl &&) = delete;

  constexpr operator const pseudo_state_decl *() const noexcept { return this; }

  // force static\global
  const pseudo_state_decl *self{this};
};

export struct pseudo_state final {
  struct null {};

  template <typename T = null> static pseudo_state make() {
    return make__<std::remove_cvref_t<T>>();
  };

  pseudo_state() = delete;

  constexpr pseudo_state(const pseudo_state_decl *state_ptr)
      : state{state_ptr} {};
  constexpr pseudo_state(const pseudo_state_decl &state_ref)
      : state{state_ref} {};

  constexpr bool operator==(const pseudo_state &other) const noexcept {
    return other.state == state;
  };

private:
  template <typename> static const pseudo_state_decl *make__() {
    static constexpr pseudo_state_decl _{};
    return _;
  };
  const pseudo_state_decl *state{make__<null>()};
};

struct hovered_r__ {
  bool hovered(uid_t uid) const { return hovered__.contains(uid); };

protected:
  std::unordered_set<uid_t> hovered__;
};

// read \ write pseudo
// read hovered

struct pseudo_base__ {
  friend struct pseudo_proxy__;

protected:
  std::unordered_map<uid_t, pseudo_state> pseudo__;
  std::unordered_map<uid_t, pseudo_state> pseudo_defer__; // sync ?
};

struct pseudo_proxy__ {
  pseudo_proxy__(uid_t uid_, pseudo_base__ &owner_)
      : uid{uid_}, owner{owner_} {};

  operator pseudo_state() const && {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid);
    } else {
      return pseudo_state::make<pseudo_state::null>();
    };
  };

  pseudo_proxy__ &operator=(pseudo_state state) && {
    owner.pseudo__.insert_or_assign(uid, state);
    return *this;
  };
  bool operator==(pseudo_state other) const && noexcept {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid) == other;
    } else {
      return false;
    };
  };

private:
  uid_t uid;
  pseudo_base__ &owner;
};

struct pseudo_r__ : virtual pseudo_base__ {
  pseudo_state pseudo(uid_t uid) const noexcept {
    if (pseudo__.contains(uid)) {
      return pseudo__.at(uid);
    } else {
      return pseudo_state::make<pseudo_state::null>();
    }
  };

  void pseudo_default(uid_t uid, pseudo_state state) {
    if (not pseudo__.contains(uid)) {
      pseudo__.insert({uid, state});
    }
  };
};

struct pseudo_rw__ : virtual pseudo_base__ {
  pseudo_proxy__ pseudo(uid_t uid) noexcept { return {uid, *this}; };
};

struct state_holder : virtual pseudo_rw__, virtual hovered_r__ {};

struct const_state_holder : virtual pseudo_r__, virtual hovered_r__ {};

struct managed_state_holder : public state_holder, public const_state_holder {
  void update(std::unordered_set<uid_t> &&hovered) {
    std::swap(hovered__, hovered);
  };
};
}; // namespace iuic
