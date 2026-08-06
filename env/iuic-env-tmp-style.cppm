// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:tmp.style;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;
import :tmp.policy;
import :tmp.memory;

namespace iuic::environment {
struct style_tmp_registry;
}
namespace iuic::utils {
template <> struct debug_api<iuic::environment::style_tmp_registry, true> {
  auto &get_sheet_debug(this auto &&self) { return self.sheet; };
};
} // namespace iuic::utils

namespace iuic::environment {

struct style_tmp_registry : private advance::interface,
                            public utils::debug_api<style_tmp_registry> {
  friend advance::interface;
  friend utils::debug_api<style_tmp_registry, true>;

private:
  struct meta {
    style::index index;
    bool is_shape_overrided{false};
    bool is_decoration_overrided{false};
    bool is_transform_overrided{false};
    bool is_freeze{false};
  };

public:
  style::value get(style::sid sid) const {
    if (auto it = index.find(sid); it != index.end()) {
      return {it->second.index, &sheet};
    } else {
      return {};
    }
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
      index.insert({++free_index, {.index = nindex, .is_freeze = true}});

      indexed_decl.insert({decl, free_index});
    }
    return indexed_decl.at(decl);
  };

  style::sid make(const style::decl &decl) {
    return make(std::addressof(decl));
  };

  bool override(style::sid sid, std::invocable<style::shape &> auto &&call) {
    if (auto it = index.find(sid); it != index.end()) {
      auto &meta = it->second;

      if (meta.is_freeze) {
        return false;
      }

      if (not meta.is_shape_overrided) {
        sheet.shape.push_back(sheet.shape.at(meta.index.shape)); // COW
        meta.index.shape = sheet.shape.size() - 1;
      }
      call(sheet.shape.at(meta.index.shape));
      return true;
    }
    return false;
  };

  bool override(style::sid sid,
                std::invocable<style::decoration &> auto &&call) {
    if (auto it = index.find(sid); it != index.end()) {
      auto &meta = it->second;

      if (meta.is_freeze) {
        return false;
      }

      if (not meta.is_decoration_overrided) {
        sheet.decoration.push_back(
            sheet.decoration.at(meta.index.decoration)); // COW
        meta.index.decoration = sheet.decoration.size() - 1;
      }

      call(sheet.decoration.at(meta.index.decoration));
      return true;
    }
    return false;
  };

  bool override(style::sid sid,
                std::invocable<style::transform &> auto &&call) {
    if (auto it = index.find(sid); it != index.end()) {
      auto &meta = it->second;

      if (meta.is_freeze) {
        return false;
      }

      if (not meta.is_transform_overrided) {
        sheet.transform.push_back(
            sheet.transform.at(meta.index.transform)); // COW
        meta.index.transform = sheet.transform.size() - 1;
      }

      call(sheet.transform.at(meta.index.transform));

      return true;
    }
    return false;
  };

  style::sid fork(style::sid sid) {
    if (auto it = index.find(sid); it != index.end()) {
      auto &meta = it->second;

      auto insert =
          index.insert({++free_index, {meta.index}}); // all flags set to false

      if (insert.second) {
        return insert.first->first;
      }
    }
    // exception ???
    return sid;
  };

  style::sid freeze(style::sid sid) {
    if (auto it = index.find(sid); it != index.end()) {
      auto &meta = it->second;

      meta.is_freeze = true;
      // restruct ???
    }
    return sid;
  };

public:
  style_tmp_registry(advance::pool &adp) { rebind(adp); };

public: // BIG-V
  style_tmp_registry(const style_tmp_registry &) = delete;
  style_tmp_registry(style_tmp_registry &&) = delete;
  style_tmp_registry &operator=(const style_tmp_registry &) = delete;
  style_tmp_registry &operator=(style_tmp_registry &&) = delete;
  ~style_tmp_registry() {};

private: // ADVANCE
  void advance() override {
    free_index = style::sid{20};
    sheet.shape.clear();
    sheet.decoration.clear();
    sheet.transform.clear();
    sheet.advence.clear();
    index.clear();
    indexed_decl.clear();
  };

private:
  style::sid free_index{20};
  style::sheet sheet;
  std::unordered_map<style::sid, meta> index;
  std::unordered_map<const style::decl *, style::sid> indexed_decl;
};
}; // namespace iuic::environment
