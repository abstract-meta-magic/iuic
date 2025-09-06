
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

namespace iuic {

void context::set_view_size(ui_size size) { ctree.set_root_size(size); };

const std::vector<relement> &context::get_tree() { return to_render; }

void context::reset() {
  ctree.reset();
  to_render.clear();
};

void context::self_size() {

  // warning

  auto rtree = ctree.reverse_range_for();

  for (auto &&cc : ctree.reverse_range_for()) {
    if (cc.is_discarted()) {
      continue;
    }
    cc.get_layout().self_size({&cc});
  }

  // отвратительно
  ctree.root().get_layout().self_size({&ctree.root()});
};
void context::childs_position() {
  // TODO : PARALLEL
  ctree.root().get_layout().set_childs_position({&ctree.root()});

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }
    cc.get_layout().set_childs_position({&cc});
  }
}

void context::balancing() {
  // TODO : PARALLEL

  for (auto &cc : ctree.range_for()) {
    cc.get_layout().balancing({&cc});
  }
};

void context::build_render_list() {
  // тут нужно будет отсеять лишние и
  // маркерованные элементы
  // после чего составить простой список
  // отрисовки

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }
    to_render.push_back(
        {.area{cc.get_rect()},
         .data{frame_render_data{.background{cc.get_style().background}}}});
  }
}

// base uid + str.hash
uid_t context::builder::make_uid(const std::string &str) const noexcept {
  return uid::make(seed.top(), str.c_str(), str.size());
};

// base uid + other.uid.hash
uid_t context::builder::make_uid(uid_t uid) const noexcept {
  return uid::make(uid);
};

// base uid + other.uid.hash + str.hash
uid_t context::builder::make_uid(uid_t uid,
                                 const std::string &str) const noexcept {
  return uid::merge(uid, make_uid(str));
};

uid_t context::builder::__make_uid_from_ptr(const void *ptr) const noexcept {
  // ptr hash ?
  if constexpr (sizeof(ptr) == sizeof(std::uint32_t)) {
    // TODO : convert ptr to uid
  } else if constexpr (sizeof(ptr) == sizeof(std::uint64_t)) {
    // TODO : convert ptr to uid
    return uid::make(reinterpret_cast<const char *>(ptr), sizeof(ptr));
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

  seed.push(uid::make(data_ptr, data_len));
  // ...
};

void context::builder::__post() noexcept {

  id.pop_back();

  seed.pop();
  uid = 0;
};

void context::builder::apply_uid(uid_t nuid) noexcept { uid = nuid; };
}; // namespace iuic
