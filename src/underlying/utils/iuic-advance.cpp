// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

module iuic.underlying.utils;
import :advance;

namespace iuic::advance {
pool &pool::global() {
  static pool _{};

  return _;
};

void pool::attach(interface *ptr) { objs.insert(ptr); }
void pool::detach(interface *ptr) { objs.erase(ptr); }
void pool::advance() {
  std::for_each(objs.begin(), objs.end(),
                [](interface *ptr) { ptr->advance(); });
};

pool::~pool() {
  for (auto *obj : objs) {
    remove(obj);
  }
};

void pool::remove(interface *ptr) { ptr->pool = nullptr; }

}; // namespace iuic::advance
