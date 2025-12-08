

module;

#include <expected>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
export module iuic.core:text.fontset;
import :base;
import :text.font;
import :style;

export namespace iuic::text {

const glyph::atlas &get_default_atlas() {
  struct ascii_decoder : glyph::decoder {
    std::expected<std::vector<glyph::index_t>, error>
    decode(const text::token &) const noexcept override {
      return {};
    };
  };

  struct native_text_binding : extern_binding {
    constexpr std::string_view info() const noexcept override {
      return "Native ASCII - text binding";
    };
  };

  static const glyph::atlas ascii_atlas{
      std::unique_ptr<glyph::decoder>{new ascii_decoder{}},
      std::unique_ptr<extern_binding>{new native_text_binding{}}};

  return ascii_atlas;
};
// shared-object
// move-only
struct fontset {
  using cref = style::font::cref;
  using decl = style::font::decl;

  fontset &&bind(cref ref, glyph::atlas &&atlas) && {
    set.insert({ref, std::move(atlas)});
    return std::move(*this);
  };

  const glyph::atlas &get_atlas(cref ref) const {
    if (set.contains(ref)) {
      return set.at(ref);
    }

    // rework for search default

    return get_default_atlas();
  };

  //  std::vector<cref> supports() const;

  operator std::shared_ptr<fontset>() && {
    return std::shared_ptr<fontset>(new fontset{std::move(*this)});
  };

private:
  struct hash {
    std::size_t operator()(const cref &ref) const noexcept {
      return ref.value().get_index();
    };
  };

  std::unordered_map<cref, glyph::atlas, hash> set{};
};

struct fontslot {
  void link(std::shared_ptr<fontset> n) { std::swap(n, set); };

  const glyph::atlas &get(style::font::cref ref) const {

    if (set) {
      return set.get()->get_atlas(ref);
    }

    return get_default_atlas();
  };

private:
  std::shared_ptr<fontset> set;
};
}; // namespace iuic::text
