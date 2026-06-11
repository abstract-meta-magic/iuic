// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.symbol:api.PUBLIC;
import std;
import :api.PRIVATE;
import :decl;

export namespace iuic::symbol {

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
