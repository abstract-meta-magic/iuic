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

scheme_handle get_scheme_handle(std::string_view scheme_name);
pack_handle get_pack_handle(std::string_view scheme_name,
                            std::string_view pack_name);
pack_handle get_pack_handl(scheme_handle scheme, std::string_view name);

pack_handle load_scheme(const std::filesystem::path &, std::string_view alias);
pack_handle load_scheme(const std::filesystem::path &, scheme_handle handl);

pack_handle load_pack(const std::filesystem::path &, pack_handle pack);
pack_handle load_pack(const std::filesystem::path &, scheme_handle scheme);
pack_handle load_pack(const std::filesystem::path &, scheme_handle scheme,
                      std::string_view alias);
pack_handle load_pack(const std::filesystem::path &,
                      std::string_view scheme_name, std::string_view alias);

bool is_scheme_loaded(std::string_view name);
bool is_scheme_loaded(scheme_handle scheme);

bool is_pack_loaded(pack_handle pack);
bool is_pack_loaded(scheme_handle scheme, std::string_view name);
bool is_pack_loaded(std::string_view scheme_name, std::string_view name);

void unload_pack(std::string_view scheme, std::string_view pack);
void unload_pack(pack_handle handl);

void unload_scheme(std::string_view name);
void unload_scheme(scheme_handle handl);

void switch_pack(pack_handle handl);
void switch_pack(scheme_handle scheme, std::string_view name);
void switch_pack(std::string_view scheme, std::string_view name);

void reset_pack(std::string_view scheme, std::string_view pack);
void reset_pack(scheme_handle handl);

std::vector<link> get_links(dynamic sym);

// TODO : normal interface
std::vector<dynamic> finds(scheme_handle, std::regex expr);

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
