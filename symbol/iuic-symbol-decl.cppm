// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.symbol:decl;
import std;

export namespace iuic::symbol {

enum class scheme_handle : std::uint64_t {};
enum class pack_handle : std::uint64_t {};

struct locked {
  std::string string_view() const;

  bool is_outdated() const;

  void retire();
};

struct link {
  locked lock() const;

  locked lock_or(std::string_view fallback) const;

  bool is_resolved() const;
};

struct dynamic {
  dynamic(std::string_view scheme, std::string_view id);
  dynamic(std::string_view scheme, std::size_t id);
  dynamic(scheme_handle scheme, std::string_view id);

  std::string string() const;

  std::string string_or(std::string_view fallback) const;

  locked lock() const;

  locked lock_or(std::string_view fallback) const;

  link link(std::string_view name);

  std::string_view unsafe_view() const;

  bool is_resolved() const;

private:
  std::uint64_t dy_id, table_id, syb_id;
};

// TODO : Make iuic.symbol:api.private_;

// TODO : Move to iuic.symbol:api.public_;

}; // namespace iuic::symbol

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
};

*/
