// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.symbol:decl;
import std;
import :handle;
import :api.PRIVATE;

export namespace iuic::symbol {

struct locked {

  bool is_valid() const { return api::is_valid_lock(lock); };

  std::string_view view() const { return api::lock_data(lock); };

  std::string_view view_or(std::string_view fallback) const {
    if (is_valid()) {
      return view();
    } else {
      return fallback;
    }
  };

  bool is_outdated() const { return api::is_lock_outdated(lock); };

  void retire() { api::retire(lock); }; // double free race ??

  // TODO : BIG-V
  locked(lock_handle lock_) : lock{lock_} {};
  locked() : lock{symbol::invalid_handle<lock_handle>} {};
  ~locked() { retire(); }
  locked(const locked &other) : lock{api::lock(other.lock)} {};
  locked &operator=(const locked &other) {
    if (std::addressof(other) != this) {
      auto tmp = lock;
      lock = api::lock(other.lock);
      api::retire(tmp);
    }

    return *this;
  };
  locked(locked &&other) : lock{symbol::invalid_handle<lock_handle>} {
    std::swap(lock, other.lock);
  };
  locked &operator=(locked &&other) {
    std::swap(lock, other.lock);
    return *this;
  };

private:
  lock_handle lock;
};

struct link {
  locked lock() const {
    return api::lock(scheme_h, api::resolve_link(scheme_h, slot_h, link_h));
  };

  bool is_resolved() const {
    return api::resolve_link(scheme_h, slot_h, link_h) !=
           symbol::invalid_handle<slot_handle>;
  };

  link(scheme_handle scheme_, slot_handle slot_, link_handle link_)
      : scheme_h{scheme_}, slot_h{slot_}, link_h{link_} {}

  link()
      : scheme_h{symbol::invalid_handle<scheme_handle>},
        slot_h{symbol::invalid_handle<slot_handle>},
        link_h{symbol::invalid_handle<link_handle>} {}

private:
  scheme_handle scheme_h;
  slot_handle slot_h;
  link_handle link_h;
};

/* символический ключ
для использования в lock
будет сделать для горячих путей
for с unresolved-link

std::vector<dynamic> dys;
key k{"font"};
for(auto dy : dys) {
  dy.link(k); <- для более быстрого разрешения
}
*/
struct key {
  // TODO : key chache system
};

/* так же как и key
используется в местах
для оптимизаций

prefetch p{"base","text::"};
for(auto str : ...) {
  dynamic dy{"base",str};
}
*/
struct prefetch {
  // TODO : prefetch chache system
};

struct dynamic {
  dynamic(std::string_view scheme_, std::string_view key)
      : scheme_h{api::reserve_scheme(scheme_)},
        slot_h{api::reserve_slot(scheme_h, key)} {};

  dynamic(scheme_handle scheme_, std::string_view key)
      : scheme_h{scheme_}, slot_h{api::reserve_slot(scheme_h, key)} {};

  std::string string() const { return std::string{lock().view()}; };

  std::string string_or(std::string_view fallback) const {
    return std::string{lock().view_or(fallback)};
  };

  locked lock() const { return api::lock(scheme_h, slot_h); };

  symbol::link link(std::string_view name) {
    return symbol::link{scheme_h, slot_h,
                        api::reserve_slot_link(scheme_h, slot_h, name)};
  };

  symbol::link link(const key &) {
    // TODO : key impl
    return {};
  };

  bool is_resolved() const { return api::is_resolved(scheme_h, slot_h); };

private:
  scheme_handle scheme_h;
  slot_handle slot_h;
};

// resolved_sym_id_eq
bool operator==(dynamic, locked);
// resolved_sym_id_eq
bool operator==(dynamic, link);
}; // namespace iuic::symbol

void hehe() {
  iuic::symbol::dynamic dy{"base", "text::main"};

  dy.lock().view_or("##test##");
  dy.link("font").lock().view_or("##test##");
};

/* example
void test() {
  using namespace iuic::text;

  // resurve and create name,but not load
  static auto scheme = i18n::get_scheme_handle("base");
  static i18n::dynamic a{scheme, "main::text"};
  static i18n::dynamic b{scheme, "main::help"};
  static i18n::dynamic c{scheme, "main::lable"};

  a.string();
  auto l = a.lock();

  std::println("{}", a.unsafe_view());
  std::println("{}", l.string_view());

  i18n::link link = a.link("next");
  i18n::locked lock = link.lock();

  if (l.is_outdated()) {
    l.retire(); // just free
    // or
    l = a.lock();
  }

  //
  i18n::load_scheme("./base.scheme", "base");
  i18n::load_pack("./base.pack", "base", "ru_RU");

  i18n::switch_pack("base", "ru_RU");

  static i18n::dynamic g{scheme, "type::cmd"};

  dynmic action = ...;

  if(auto t = action.link("type");type == g) {

  }
};

*/
