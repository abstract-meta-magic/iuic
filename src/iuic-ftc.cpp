

module;

#include <limits>
#include <vector>

module iuic.core;
import :ftc;

namespace iuic {

std::vector<computing_context> &FTC::get() { return nodes; };
computing_context &FTC::get_parent(computing_context &other) {
  if (other.parent == std::numeric_limits<size_t>::max()) {
    return root;
  }

  return nodes[other.parent];
};

}; // namespace iuic
