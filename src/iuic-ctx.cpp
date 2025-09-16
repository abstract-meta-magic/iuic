
module;
#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <iterator>
#include <print>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

module iuic.core;
import :hash;

namespace iuic {

void context::set_view_size(ui_size size) { ctree.set_root_size(size); };

const std::vector<relement> &context::get_tree() { return to_render; }

void context::reset() {
  ctree.reset();
  to_render.clear();
  storage.advance_generation();
};

void context::proccess_measure() {

  // warning

  auto rtree = ctree.reverse_range_for();

  for (auto &&cc : ctree.reverse_range_for()) {
    if (cc.is_discarted()) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            auto res = layout->measure({&cc});

            // TODO : fix me
            if (res) {
              cc.apply(res.value().rq); // this wrong. no check measure result
            } else {
              cc.discard();
            }
          }
        },
        cc.get_layout());
  }

  // mda root calc
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          auto res = layout->measure({&ctree.root()});

          if (res) {
            ctree.root().apply(res.value().rq);
          } else {
            ctree.root().discard();
          };
        }
      },
      ctree.root().get_layout());
};

void context::proccess_position() {
  // TODO : PARALLEL
  ctree.root().apply(ui_position{0, 0});
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->position({&ctree.root()});
        }
      },
      ctree.root().get_layout());

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            layout->position({&cc});
          }
        },
        cc.get_layout());
  }
}

void context::proccess_arrange() {
  // TODO : PARALLEL
  auto &max_size = ctree.root().get_style().shape.max_size;
  ctree.root().apply(
      ui_size{std::get<upixel_t>(max_size.w), std::get<upixel_t>(max_size.h)});

  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->arrange({&ctree.root()});
        }
      },
      ctree.root().get_layout());

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            layout->arrange({&cc});
          }
        },
        cc.get_layout());
  }
};

void context::build_render_list() {
  // тут нужно будет отсеять лишние и
  // маркерованные элементы
  // после чего составить простой список
  // отрисовки

  for (auto &cc : ctree.range_for()) {
    auto rect = cc.get_rect();
    if (not rect) {
      // err ?
      continue;
    }

    to_render.push_back(
        {.area{rect.value()},
         .data{frame_render_data{.background{cc.get_style().background}}}});
  }
}

// base uid + str.hash
uid_t context::builder::make_uid(const std::string &str) const noexcept {
  return hash::make(seed.top(), str.c_str(), str.size());
};

// base uid + other.uid.hash + str.hash
uid_t context::builder::make_uid(uid_t uid,
                                 const std::string &str) const noexcept {
  return hash::merge(uid, make_uid(str));
};

uid_t context::builder::__make_uid_from_ptr(const void *ptr) const noexcept {
  // ptr hash ?
  if constexpr (sizeof(ptr) == sizeof(std::uint32_t)) {
    // TODO : convert ptr to uid
  } else if constexpr (sizeof(ptr) == sizeof(std::uint64_t)) {
    // TODO : convert ptr to uid
    return hash::make(seed.top(), reinterpret_cast<const char *>(ptr),
                      sizeof(ptr));
  } else {
    throw "Unsupported pointer size";
  };
};

uid_t context::builder::__make_base_uid() const noexcept { return {}; };

void context::builder::__prev() noexcept {
  id.push_back(ctx.ctree.index_at_last());

  // ...
  const char *data_ptr = reinterpret_cast<const char *>(&id[0]);

  auto data_len = sizeof(size_t) * id.size();

  seed.push(hash::make(data_ptr, data_len));
  // ...
};

void context::builder::__post() noexcept {

  id.pop_back();

  seed.pop();
  uid = 0;
};

void context::builder::apply_uid(uid_t nuid) noexcept { uid = nuid; };
}; // namespace iuic
