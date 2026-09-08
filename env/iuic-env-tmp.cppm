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

  struct clip_t : advance::interface {
    struct clipzone_t {
      static constexpr auto none = units::clip_id{
          std::numeric_limits<std::underlying_type_t<units::clip_id>>::max()};
      units::ui::area area{};
      style::value style{0};
      units::clip_id base{none};
    };

    units::clip_id make_clipzone(const units::ui::area &area,
                                 style::value style) {
      units::clip_id res{zone.size()};
      zone.push_back({area, style});
      return res;
    };

    units::clip_id make_clipzone(const units::ui::area &area,
                                 style::value style, units::clip_id id) {
      units::clip_id res{zone.size()};
      zone.push_back({area, style, id});
      return res;
    };

    std::span<const clipzone_t> get() const { return zone; };

    // get ?
  private:
    void advance() override { zone.clear(); };

  private:
    std::vector<clipzone_t> zone;
  } clip;

  struct {
    units::ui::size viewport_size{units::upixel{0}, units::upixel{0}};
    units::upixel segment_size{4};
  } meta; // frame meta

public: // ctor
  tmp(advance::pool &adp) : event{adp}, style{adp}, policy{adp}, memory{adp} {
    clip.rebind(adp);
  };

public: // BIG-V
  tmp(const tmp &) = delete;
  tmp(tmp &&) = delete;
  tmp operator=(const tmp &) = delete;
  tmp operator=(tmp &&) = delete;
  ~tmp() {};
};
}; // namespace iuic::environment
