// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:atlas;
import std;
import iuic.underlying;
import :glyph;

namespace iuic::text {
struct atlas_registry_handler__;

struct atlas_glyph_map__ {
  std::map<glyph::id_t, units::upixel> advance;
  std::map<std::uint64_t, units::pixel> kerning;
};

struct atlas_meta_map__ {
  bool is_monospace : 1 {false};
  bool kerning_support : 1 {false};
  bool scale_support : 1 {false};
  bool separation_support : 1 {false};
  // etc...
};
}; // namespace iuic::text

export namespace iuic::text {

enum class directional { RTL, LTR, TTB, BTT };

struct decoder {
  struct encoding_not_supported : std::runtime_error {
    encoding_not_supported()
        : std::runtime_error{"This decoder does not support this encoding."} {};
  };

  struct capabilities_t {
    bool is_std_char_support() const { return c__.test(0); };
    bool is_utf8_char_support() const { return c__.test(1); };
    bool is_utf16_char_support() const { return c__.test(2); };
    bool is_utf32_char_support() const { return c__.test(3); };
    bool is_raw_byte_support() const { return c__.test(4); };

    capabilities_t &set_std_char_support(bool v_ = true) {
      return c__.set(0, v_), *this;
    };
    capabilities_t &set_utf8_char_support(bool v_ = true) {
      return c__.set(1, v_), *this;
    };
    capabilities_t &set_utf16_char_support(bool v_ = true) {
      return c__.set(2, v_), *this;
    };
    capabilities_t &set_utf32_char_support(bool v_ = true) {
      return c__.set(3, v_), *this;
    };
    capabilities_t &set_raw_byte_support(bool v_ = true) {
      return c__.set(4, v_), *this;
    };

    // other

    // ctor
  private:
    std::bitset<8> c__;
  };

  virtual ~decoder() {};

  virtual std::vector<glyph::id_t> decode(std::string_view) {
    throw encoding_not_supported{};
  };
  virtual std::vector<glyph::id_t> decode(std::u8string_view) {
    throw encoding_not_supported{};
  };
  virtual std::vector<glyph::id_t> decode(std::u16string_view) {
    throw encoding_not_supported{};
  };
  virtual std::vector<glyph::id_t> decode(std::u32string_view) {
    throw encoding_not_supported{};
  };
  virtual std::vector<glyph::id_t> decode(std::span<const std::byte>) {
    throw encoding_not_supported{};
  };
  virtual const capabilities_t &capabilities() const = 0;

  // virtual const glyph::id_t idle() const { return 0; };

  // virtual const glyph::id_t undefined() const { return 0; };
};

struct atlas final {
private:
  struct builder;

public:
  struct glyph_not_exist : std::runtime_error {
    glyph_not_exist(std::string_view name, glyph::id_t id)
        : std::runtime_error{std::format("Atlas [{}] cen't contains glyph with "
                                         "id: {} \n This encode error.",
                                         name, id)} {};
  };
  using id = std::size_t;

  union { // ignore inner offset
    struct : utils::adv_member_for<atlas> {
      units::upixel operator[](glyph::id_t id) const {
        if (auto advance = self().glyph__.advance.find(id);
            advance != self().glyph__.advance.end()) {
          return advance->second;
        }
        throw glyph_not_exist{self().name, id};
      };
    } advance [[no_unique_address]]{};
    struct : utils::adv_member_for<atlas> {
      units::pixel operator[](glyph::id_t lhs, glyph::id_t rhs) const {
        static units::pixel decoy{0}; // TODO NORMAL IMPL

        std::uint64_t key = (static_cast<std::uint64_t>(lhs) << 32) | rhs;

        if (auto kerning = self().glyph__.kerning.find(key);
            kerning != self().glyph__.kerning.end()) {
          return kerning->second;
        };

        return decoy;
      };
    } kerning [[no_unique_address]];
  } glyphs [[no_unique_address]]{};

  bool is_monospace() const { return true; };

  bool is_scale_supported() const { return meta__.scale_support; };

  bool is_kerning_supported() const { return meta__.kerning_support; };

  std::string name;
  external::instance binding;
  std::unique_ptr<decoder> decoder;

  // directional

  // Need to compute layout
  units::upixel text_height;
  // Need to compute layout
  units::upixel baseline_offset;

  static const atlas &by_name(std::string_view name);
  static const atlas &by_symname(std::string_view name);
  static const atlas &by_family(std::string_view name);
  static const atlas &by_id(id id);

  // static void set_symname(std::string_view symname,atlas);
  // static void reset_symname(std::string_view symname);

  static constexpr id invalid_id{std::numeric_limits<std::size_t>::max()};
  static id get_id(const atlas &);
  id get_id() const;

  // invalide atlas ctor
  atlas()
      : binding{}, decoder{nullptr}, name{"Invalid"},
        handler__{nullptr, registry_detach} {};

  // TODO : BIG-V
  atlas(const atlas &) = delete;
  atlas &operator=(const atlas &) = delete;
  atlas(atlas &&other)
      : name{std::move(other.name)}, glyph__{std::move(other.glyph__)},
        meta__{std::move(other.meta__)}, handler__{nullptr, registry_detach} {
    std::swap(decoder, other.decoder);
    std::swap(binding, other.binding);
    std::swap(text_height, other.text_height);
    std::swap(baseline_offset, other.baseline_offset);
    std::swap(handler__, other.handler__);
    registry_rebind(this, handler__.get());
  };

  atlas &operator=(atlas &&other) {
    if (std::addressof(other) == this) {
      return *this;
    }
    std::swap(name, other.name);
    std::swap(glyph__, other.glyph__);
    std::swap(meta__, other.meta__);
    std::swap(decoder, other.decoder);
    std::swap(binding, other.binding);
    std::swap(text_height, other.text_height);
    std::swap(baseline_offset, other.baseline_offset);
    std::swap(handler__, other.handler__);
    registry_rebind(this, handler__.get());
    return *this;
  };
  ~atlas() {};

  static builder construct(std::string_view name_);

private:
  // can thow exception
  atlas(std::string_view &&name_, atlas_glyph_map__ &&glyph_,
        atlas_meta_map__ &&meta_)
      : name{std::move(name_)}, glyph__{std::move(glyph_)},
        meta__{std::move(meta_)}, handler__{nullptr, registry_detach} {
    handler__.reset(registry_attach(this));
  };

  static atlas_registry_handler__ *registry_attach(const atlas *);
  static void registry_detach(atlas_registry_handler__ *);
  static void registry_rebind(const atlas *, atlas_registry_handler__ *);

private:
  atlas_glyph_map__ glyph__;
  atlas_meta_map__ meta__;
  //------- registry
  std::unique_ptr<atlas_registry_handler__,
                  void (*)(atlas_registry_handler__ *)>
      handler__;
};

struct atlas::builder {
  struct fill_proxy {
    void link_advance(text::glyph::id_t id, units::upixel adv) {
      map.advance.insert({id, adv});
    };

    void link_kerning(text::glyph::id_t lhs, text::glyph::id_t rhs,
                      units::pixel ker) {
      std::uint64_t key = (static_cast<std::uint64_t>(lhs) << 32) | rhs;
      map.kerning.insert({key, ker});
    };

    fill_proxy(atlas_glyph_map__ &map_) : map{map_} {};

  private:
    atlas_glyph_map__ &map;
  };

  builder &set_monospace() { return meta__.is_monospace = true, *this; };

  template <typename T>
  builder &set_decoder(T &&decoder)
    requires std::is_base_of_v<iuic::text::decoder, T>
  {
    decoder__.reset(new T{std::forward<T>(decoder)});
    return *this;
  };

  template <typename T> builder &set_binding(T &&binding) {
    binding__ = std::forward<T>(binding);
    return *this;
  };

  builder &set_kerning_supported() {
    return meta__.kerning_support = true, *this;
  };

  builder &set_text_height(iuic::units::upixel height) {
    text_height__ = height;
    return *this;
  };
  builder &set_baseline_offset(iuic::units::upixel offset) {
    baseline_offset__ = offset;
    return *this;
  };

  builder &fill_glyph_map(auto &&call) {
    return call(fill_proxy{glyph__}), *this;
  };

  atlas finalize() {
    atlas res{std::move(name), std::move(glyph__), std::move(meta__)};

    res.baseline_offset = baseline_offset__;
    res.text_height = text_height__;
    res.decoder.swap(decoder__);
    std::swap(res.binding, binding__);

    return res;
  };

  builder(std::string name_) : name{name_} {};

private:
  std::unique_ptr<iuic::text::decoder> decoder__;
  iuic::external::instance binding__;
  std::string name;
  iuic::units::upixel text_height__{8};
  iuic::units::upixel baseline_offset__{8};
  atlas_glyph_map__ glyph__;
  atlas_meta_map__ meta__;
};

atlas::builder atlas::construct(std::string_view name_) {
  return {std::string{name_}};
};

}; // namespace iuic::text
