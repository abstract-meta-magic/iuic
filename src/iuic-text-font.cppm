

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
  glyph::index_t id;
  ui_position position;
};

struct glyph::atlas {
  atlas(const atlas &) = delete;
  atlas &operator=(const atlas &) = delete;
  atlas(atlas &&) = default;
  atlas &operator=(atlas &&) = default;

  // see iuic::glyph::atlas protocol object
  std::span<const std::byte> get_proto_obj() const;

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
    return r.first + r.second;
  };

  std::unordered_map<std::pair<index_t, index_t>, ui_position, decltype(phash)>
      kerning;
  std::vector<glyph> cpu_present; // [][]
  std::vector<std::byte> proto_object;
};

struct glyph::decoder {
  struct error;

  virtual ~decoder() = default;

  virtual std::expected<std::vector<index_t>, error>
  decode(const iuic::text::token &) const noexcept = 0;
};
}; // namespace iuic::text
