module;

#include <memory>

module iuic.core;
import :text.font;

namespace iuic::text {

glyph::atlas::atlas(std::unique_ptr<decoder> &&d_) : decoder_{std::move(d_)} {};
glyph::atlas::atlas(std::unique_ptr<decoder> &&d_,
                    std::unique_ptr<extern_binding> &&e_)
    : decoder_{std::move(d_)}, binding{std::move(e_)} {};
}; // namespace iuic::text
