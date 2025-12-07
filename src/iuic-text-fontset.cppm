

module;

#include <memory>
#include <unordered_map>
#include <vector>
export module iuic.core:text.fontset;
import :base;
import :text.font;
import :style;

namespace iuic::text {

// shared-object
// move-only
struct fontset {
  using cref = style::font::cref;
  using decl = style::font::decl;

  fontset &&bind(cref, glyph::atlas &&) &&;

  const glyph::atlas &get_atlas(cref ref) const;

  std::vector<cref> supports() const;

  operator std::shared_ptr<fontset>() && {
    return std::shared_ptr<fontset>(new fontset{std::move(*this)});
  };

private:
  using hash = decltype([](const cref &cref) -> std::size_t {
    return cref.value().get_index();
  });

  std::unordered_map<cref, glyph::atlas, hash> set;
};

struct fontslot {

  void link(std::shared_ptr<fontset>);

  const glyph::atlas &get(style::font::cref) const;

  const glyph::atlas &get(const style::font::decl &) const;

private:
  std::shared_ptr<fontset> set;
};
}; // namespace iuic::text
