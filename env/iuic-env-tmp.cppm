// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:tmp;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;
export import :tmp.exception;
import :tmp.policy;
import :tmp.style;

namespace iuic::environment {}; // namespace iuic::environment

export namespace iuic::environment {
struct tmp {
  event::hub event;

  style_tmp_registry style;

  policy_tmp_registry policy;

  tmp_memory memory;

  struct {
    units::ui::size viewport_size{units::upixel{0}, units::upixel{0}};
    units::upixel segment_size{4};
  } meta; // frame meta

public: // ctor
  tmp(advance::pool &adp) : event{adp}, style{adp}, policy{adp}, memory{adp} {};

public: // BIG-V
  tmp(const tmp &) = delete;
  tmp(tmp &&) = delete;
  tmp operator=(const tmp &) = delete;
  tmp operator=(tmp &&) = delete;
  ~tmp() {};
};
}; // namespace iuic::environment
