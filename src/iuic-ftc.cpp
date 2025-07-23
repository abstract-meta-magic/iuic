

module;

#include <vector>

module iuic.core;
import :ftc;

namespace iuic {

std::vector<FTC::node> &FTC::get() { return nodes; };
}; // namespace iuic
