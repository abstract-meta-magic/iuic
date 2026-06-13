// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.symbol:api.PRIVATE;
import std;
import :handle;

namespace iuic::symbol::api {

scheme_handle reserve_scheme(std::string_view);

slot_handle reserve_slot(scheme_handle, std::string_view);

// return link id
link_handle reserve_slot_link(scheme_handle, slot_handle, std::string_view str);

std::size_t slot_links_count(scheme_handle, slot_handle);

slot_handle resolve_link(scheme_handle, slot_handle, link_handle);

bool is_resolved(scheme_handle);

bool is_resolved(scheme_handle, slot_handle);

bool is_resolved(scheme_handle, slot_handle, link_handle);

// LOCK
lock_handle lock(scheme_handle, slot_handle);

lock_handle lock(lock_handle);

void retire(lock_handle);

bool is_valid_lock(lock_handle);

// TODO : rename ???
bool is_lock_outdated(lock_handle);

std::string_view lock_data(lock_handle);

// is relolved
// get_data
// etc

}; // namespace iuic::symbol::api
