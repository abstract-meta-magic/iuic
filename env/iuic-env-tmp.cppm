// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:tmp;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;

export namespace iuic::environment {
struct tmp : iuic::advance::interface {
  struct : private advance::interface {
    friend tmp;

  private:
    struct meta {
      style::index index;
      bool fork{false};
      // override info
      // fork of
      // decl info
    };

  public:
    style::value get(style::sid sid) const {
      if (auto it = index.find(sid); it != index.end()) {
        return {it->second.index, &sheet};
      } else {
        return {};
      }
    };

    // fork ?
    style::sid make(style::sid sid) {
      if (index.contains(sid)) {
        index.insert({++free_index, index.at(sid)});
      }

      return free_index;
    };

    style::sid make(const style::decl *decl) {
      if (not indexed_decl.contains(decl)) {
        // stage 1
        style::index nindex;
        sheet.shape.push_back(decl->shape);
        sheet.decoration.push_back(decl->decoration);
        sheet.transform.push_back(decl->transform);
        sheet.advence.push_back(decl->advanced);
        nindex.shape = sheet.shape.size() - 1;
        nindex.decoration = sheet.decoration.size() - 1;
        nindex.transform = sheet.transform.size() - 1;
        nindex.advance = sheet.advence.size() - 1;
        // stage 2
        index.insert({++free_index, {nindex}});

        indexed_decl.insert({decl, free_index});
      }
      return indexed_decl.at(decl);
    };

    style::sid make(const style::decl &decl) {
      return make(std::addressof(decl));
    };

    style::sid override(style::sid sid, style::shape &&shape) {

      if (index.contains(sid)) {
        if (not index.at(sid).fork) {
          index.insert({++free_index, {index.at(sid).index, true}});
          sid = free_index;
        }

        auto &meta = index.at(sid);
        sheet.shape.push_back(std::move(shape));
        meta.index.shape = sheet.shape.size() - 1;
      }

      return sid;
    };

    style::sid override(style::sid sid, style::decoration &&decoracion) {
      if (index.contains(sid)) {
        if (not index.at(sid).fork) {
          index.insert({++free_index, {index.at(sid).index, true}});
          sid = free_index;
        }

        auto &meta = index.at(sid);
        sheet.decoration.push_back(std::move(decoracion));
        meta.index.decoration = sheet.decoration.size() - 1;
      }

      return sid;
    };

    style::sid override(style::sid sid, style::transform &&transform) {
      if (index.contains(sid)) {
        if (not index.at(sid).fork) {
          index.insert({++free_index, {index.at(sid).index, true}});
          sid = free_index;
        }

        auto &meta = index.at(sid);
        sheet.transform.push_back(std::move(transform));
        meta.index.transform = sheet.transform.size() - 1;
      }

      return sid;
    };

    style::sid override(style::sid sid, style::advanced &&advance) {
      if (index.contains(sid)) {
        if (not index.at(sid).fork) {
          index.insert({++free_index, {index.at(sid).index, true}});
          sid = free_index;
        }

        auto &meta = index.at(sid);
        sheet.advence.push_back(std::move(advance));
        meta.index.advance = sheet.advence.size() - 1;
      }

      return sid;
    };

  private:
    style::sid free_index{20};
    style::sheet sheet;
    std::unordered_map<style::sid, meta> index;
    std::unordered_map<const style::decl *, style::sid> indexed_decl;
  } style;

  event::hub event;

  struct : private advance::interface {
    friend tmp;

    struct policy {
      const erasure::type *type;
      std::uint64_t value;
      units::uid uid;
    };

    void set(units::uid uid, auto p)
      requires(sizeof(p) < sizeof(std::uint64_t) && std::is_enum_v<decltype(p)>)
    {
      policy el;
      el.value = std::to_underlying(p);
      el.uid = uid;
      el.type = erasure::type::from<decltype(p)>();

      if (auto it = std::find_if(
              data.begin(), data.end(),
              [&](policy &p) { return el.uid == p.uid && el.type == p.type; });
          it != data.end()) {
        (*it).value = std::to_underlying(p);
      } else {
        data.push_back(el);
      }
    };

    template <typename T>
    T get(units::uid uid)
      requires(sizeof(T) < sizeof(std::uint64_t) && std::is_enum_v<T>)
    {

      auto *type = erasure::type::from<T>();
      if (auto it = std::find_if(
              data.begin(), data.end(),
              [&](policy &p) { return uid == p.uid && type == p.type; });
          it != data.end()) {

        return static_cast<T>(
            static_cast<std::underlying_type_t<T>>(it->value));
      } else {
        return {};
      };
    };

    void advance() override { data.clear(); };

  private:
    std::vector<policy> data;
  } policy;

  static constexpr std::size_t tmp_buff_size = 1024 * 1024 * 4;

  struct : private advance::interface {
    friend tmp;

    template <typename T> std::span<T> allocate(std::size_t count = 1) {
      return {static_cast<T *>(allocate(sizeof(T), alignof(T), count)), count};
    };

  private:
    void *allocate(std::size_t size, std::size_t align, std::size_t count = 1) {
      return allocator.allocate(size * count, align);
    };
    void advance() override { allocator.release(); };

  private:
    std::array<std::byte, iuic::cenv::num("iuic::sizeof::buff::tmp")
                              .value_or(1024 * 1024 * 4)>
        tmp_buff;
    std::pmr::monotonic_buffer_resource allocator{&tmp_buff, tmp_buff.size()};

  } memory;

  struct {
    units::ui::size viewport_size{units::upixel{0}, units::upixel{0}};
    units::upixel segment_size{4};
  } meta; // frame meta

  tmp(advance::pool &adp) : event{adp} {
    rebind(adp);
    policy.rebind(adp);
    style.rebind(adp);
    memory.rebind(adp);
  };

private:
};
}; // namespace iuic::environment
