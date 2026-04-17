// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:environment.tmp;
import iuic.underlying;
import iuic.state;
import :machine.dispatcher;
import :key_code;
import :style;
import :event.collector;
import :policy;

namespace iuic::environment {
struct tmp : iuic::advance::interface {
  struct {
  } text;

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
        sheet.advence.push_back(decl->advence);
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

    style::sid override(style::sid sid, style::advence &&advance) {
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

  struct : private advance::interface {
    friend tmp;
    void attach(event::value e) { data[e.uid].push_back(e); };

    std::span<event::value> list_of(units::uid uid) {
      if (auto it = data.find(uid); it != data.end()) {
        return it->second;
      } else {
        return {};
      };
    };

  private:
    void advance() override { data.clear(); };

  private:
    std::map<units::uid, std::vector<event::value>> data;
  } event;

  struct : private advance::interface {
    friend tmp;
    policy::hovered hovered(units::uid uid) {
      if (data.contains(uid)) {
        return data.at(uid).first;
      } else {
        return {};
      }
    };

    policy::event event(units::uid uid) {
      if (data.contains(uid)) {
        return data.at(uid).second;
      } else {
        return {};
      }
    };

    void set(units::uid uid, policy::hovered h) {
      if (data.contains(uid)) {
        data.at(uid).first = h;
      } else {
        data.insert({uid, {h, {}}});
      }
    };
    void set(units::uid uid, policy::event e) {
      if (data.contains(uid)) {
        data.at(uid).second = e;
      } else {
        data.insert({uid, {{}, e}});
      }
    };

  private:
    void advance() override { data.clear(); };

  private:
    std::map<units::uid, std::pair<policy::hovered, policy::event>> data;
  } policy;

  static constexpr std::size_t tmp_buff_size = 1024 * 1024 * 4;

  struct : private advance::interface {
    friend tmp;
    void *allocate(std::size_t size, std::size_t align, std::size_t count = 1) {
      return allocator.allocate(size * count, align);
    };

    template <typename T>
    std::span<T> allocate(std::size_t count = 1)
      requires std::is_trivially_destructible_v<T>
    {
      return {static_cast<T *>(allocate(sizeof(T), alignof(T), count)), count};
    };

  private:
    void advance() override { allocator.release(); };

  private:
    std::array<std::byte, tmp_buff_size> tmp_buff;
    std::pmr::monotonic_buffer_resource allocator{&tmp_buff, tmp_buff_size};

  } memory;

  struct {
    units::ui::size viewport_size{0, 0};
  } meta; // frame meta

  tmp(advance::pool &adp) {
    rebind(adp);

    policy.rebind(adp);
    style.rebind(adp);
    memory.rebind(adp);
  };

private:
};
}; // namespace iuic::environment
