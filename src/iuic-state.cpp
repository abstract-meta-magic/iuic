

module iuic.core;
import :state;

namespace iuic {
// *** PSEUDO--PROXY ***
pseudo_proxy__::operator pseudo_state() const noexcept {
  if (owner.pseudo__.contains(uid)) {
    return owner.pseudo__.at(uid).to;
  } else {
    return pseudo_state::null();
  };
};

pseudo_proxy__ &pseudo_proxy__::operator=(pseudo_state nstate) noexcept {

  if (owner.pseudo__.contains(uid)) {
    auto &state = owner.pseudo__.at(uid);

    state.last_change = time_t::clock::now();
    state.old = state.from;
    state.from = state.to;
    state.to = nstate;
  } else {
    owner.pseudo__.insert({uid, transition_info{{time_t::clock::now()},
                                                {pseudo_state::null()},
                                                {pseudo_state::null()},
                                                nstate}});
  };
  return *this;
};

bool pseudo_proxy__::operator==(pseudo_state other) const noexcept {
  if (owner.pseudo__.contains(uid)) {
    return owner.pseudo__.at(uid).to == other;
  } else {
    return false;
  };
}
pseudo_state pseudo_proxy__::from() const noexcept {
  if (owner.pseudo__.contains(uid)) {
    return owner.pseudo__.at(uid).from;
  } else {
    return pseudo_state::null();
  };
};

pseudo_state pseudo_proxy__::to() const noexcept {
  if (owner.pseudo__.contains(uid)) {
    return owner.pseudo__.at(uid).to;
  } else {
    return pseudo_state::null();
  };
};

time_t pseudo_proxy__::time_point() const noexcept {
  if (owner.pseudo__.contains(uid)) {
    return owner.pseudo__.at(uid).last_change;
  } else {
    return time_t{};
  }
};

// *** CONST-HOLDER ***

bool const_state_holder::hovered(uid_t uid) const noexcept {
  return hovered__.contains(uid);
};

pseudo_state const_state_holder::pseudo(uid_t uid) const noexcept {
  return pseudo__.contains(uid) ? pseudo__.at(uid).to : pseudo_state::null();
};

void const_state_holder::pseudo_init_value(uid_t uid,
                                           pseudo_state state_) noexcept {
  if (not pseudo__.contains(uid) ||
      pseudo__.at(uid).to == pseudo_state::null()) {
    pseudo__.insert_or_assign(uid, transition_info{.to = state_});
  }
}

pseudo_proxy__ state_holder::pseudo(uid_t uid) noexcept {
  return {uid, *this};
};

}; // namespace iuic
