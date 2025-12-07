

module;

#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

export module iuic.core:text.font;
import :base;
import :text.token;

export namespace iuic::text {

struct glyph {
  upm_t width;
  upm_t height;
  upm_t advance;
  upm_t vertical_offset;
  upm_t horisontal_offset;
  struct decoder;
  struct atlas;
  struct placement;
  using index_t = std::uint32_t;
  using sequence = std::span<const placement>;
};

struct glyph::placement {
  glyph::index_t id{0};
  float scale{1.0f};
  ui_position position{0, 0};
};

// TODO : error code
static_assert(sizeof(glyph::placement) == 16, "...");

struct glyph::atlas {
  atlas(std::unique_ptr<decoder> &&d_);
  atlas(std::unique_ptr<decoder> &&d_, std::unique_ptr<extern_binding> &&e_);
  atlas(const atlas &) = delete;
  atlas &operator=(const atlas &) = delete;
  atlas(atlas &&) = default;
  atlas &operator=(atlas &&) = default;

  // see iuic::glyph::atlas protocol object
  const extern_binding &get_binding() const { return *binding; };

  bool is_monospace() const { return kerning.empty(); };

  ui_position get_kerning(index_t l, index_t r) const {
    if (kerning.contains({l, r})) {
      return kerning.at({l, r});
    }

    return {0, 0};
  };

  const glyph *get_glyph(index_t i) const {
    if (cpu_present.size() > i) {
      return &cpu_present[i];
    }

    return nullptr;
  };

  const decoder &get_decoder() const { return *decoder_; };

  const upixel_t &base_height() const { return height; };

  // monospace - font ??

  // возможность сброса proto_object
private:
  std::unique_ptr<decoder> decoder_;
  upixel_t height; // высота строки

  static constexpr auto phash = [](auto &&r) -> std::size_t {
    return r.first ^ (0 << r.second);
  };

  std::unordered_map<std::pair<index_t, index_t>, ui_position, decltype(phash)>
      kerning;
  std::vector<glyph> cpu_present; // [][]
  std::unique_ptr<extern_binding> binding{new extern_binding{}};
};

struct glyph::decoder {
  struct error {};

  virtual ~decoder() = default;

  virtual std::expected<std::vector<index_t>, error>
  decode(const iuic::text::token &) const noexcept = 0;
};

}; // namespace iuic::text
