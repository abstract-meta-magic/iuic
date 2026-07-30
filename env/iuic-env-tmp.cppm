// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:tmp;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
import iuic.event;
import :tmp.policy;

export namespace iuic::exception {
struct try_override_freeze : std::logic_error {
  try_override_freeze() : std::logic_error{"Try override freeze object"} {};
};

struct tmp_buffer_overflow : std::overflow_error {
  tmp_buffer_overflow() : std::overflow_error{"TMP BUFFER OVERFLOW"} {};
};

}; // namespace iuic::exception

namespace iuic::environment {

std::pmr::memory_resource *buffer_overflow_ex_resource() {
  struct : public std::pmr::memory_resource {
  protected:
    void *do_allocate(std::size_t, std::size_t) override {
      throw iuic::exception::tmp_buffer_overflow{};
    };

    void do_deallocate(void *, std::size_t, std::size_t) override {};

    bool do_is_equal(
        const std::pmr::memory_resource &other) const noexcept override {
      return false;
    };
  } static _;

  return &_;
};

}; // namespace iuic::environment

export namespace iuic::environment {
struct tmp : iuic::advance::interface {
  struct : private advance::interface {
    friend tmp;

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

        auto insert = index.insert(
            {++free_index, {meta.index}}); // all flags set to false

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

  private:
    style::sid free_index{20};
    style::sheet sheet;
    std::unordered_map<style::sid, meta> index;
    std::unordered_map<const style::decl *, style::sid> indexed_decl;
  } style;

  event::hub event;

  policy_storage policy;

  static constexpr std::size_t tmp_buff_size = 1024 * 1024 * 4;

  struct : private advance::interface {
    friend tmp;

    template <typename T> std::span<T> allocate(std::size_t count = 1) {
      return {static_cast<T *>(allocate(sizeof(T), alignof(T), count)), count};
    };

  private:
    void *allocate(std::size_t size, std::size_t align, std::size_t count = 1) {
      // throw
      return allocator.allocate(size * count, align);
    };

    void advance() override { allocator.release(); };

  private:
    std::array<std::byte, iuic::cenv::num("iuic::sizeof::buff::tmp")
                              .value_or(1024 * 1024 * 4)>
        tmp_buff;
    std::pmr::monotonic_buffer_resource allocator{
        &tmp_buff, tmp_buff.size(), buffer_overflow_ex_resource()};

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
