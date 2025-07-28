

module;
#include <iostream>
module iuic.core;

import :base;

namespace iuic {

celement &childs_set::get() {
  if (not has_next()) {
    throw "HAHA";
  }
  auto &res = (*container)[chs->front()];

  chs->pop_front();

  return res;
}

const celement &childs_set::get() const {
  if (not has_next()) {
    throw "HAHA";
  }
  auto res = container->cbegin() + chs->front();

  std::cout << "hh" << std::endl;

  std::cout << chs->size() << std::endl;
  std::cout << chs->front() << std::endl;

  chs->pop_front();

  return *res;
}
}; // namespace iuic
