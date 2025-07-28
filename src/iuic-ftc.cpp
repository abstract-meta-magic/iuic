

module;

#include <vector>

module iuic.core;
import :ftc;

namespace iuic {

std::vector<celement> &FTC::get() { return elements; };

FTC::node &FTC::get(size_t id) { return nodes[id]; };
}; // namespace iuic
