// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:atlas;
import std;
import iuic.underlying;
import :glyph;

namespace iuic::text {
struct atlas_registry_handler__;

struct atlas_glyph_map__ {
  text::glyph::id_t etalon{0};
  std::map<glyph::id_t, text::glyph> map;
};

struct atlas_meta_map__ {
  bool is_monospace : 1 {false};
  // etc...
};
}; // namespace iuic::text

export namespace iuic::text {

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

  struct : utils::adv_member_for<atlas> {
    const text::glyph &operator[](glyph::id_t id) const {
      if (auto glyph = self().glyph__.map.find(id);
          glyph != self().glyph__.map.end()) {
        return glyph->second;
      }
      throw glyph_not_exist{self().name, id};
    };
  } glyph [[no_unique_address]];

  bool is_monospace() const { return true; };

  const text::glyph &etalon() const { return glyph[glyph__.etalon]; };

  std::unique_ptr<external::binding> binding;
  std::unique_ptr<decoder> decoder;
  const std::string name;

  static const atlas &by_name(std::string_view name);
  static const atlas &by_symname(std::string_view name);
  static const atlas &by_family(std::string_view name);
  static const atlas &by_id(id id);

  // static void make_famaly(std::string_view famaly_name,atlas...);
  // static void set_symname(std::string_view symname,atlas);
  // static void reset_symname(std::string_view symname);

  static constexpr id invalid_id{std::numeric_limits<std::size_t>::max()};
  static id get_id(const atlas &);
  id get_id() const;

  // invalide atlas ctor
  atlas()
      : binding{nullptr}, decoder{nullptr}, name{"Invalid"},
        handler__{nullptr, registry_detach} {};

  // TODO : BIG-V
  atlas(const atlas &) = delete;
  atlas &operator=(const atlas &) = delete;
  atlas(atlas &&) = delete;            // can be
  atlas &operator=(atlas &&) = delete; // can be
  ~atlas() {};

  static builder construct(std::string_view name_);

private:
  // can thow exception
  atlas(std::string_view &&name_, atlas_glyph_map__ &&glyph_,
        atlas_meta_map__ &&meta_)
      : name{std::move(name_)}, glyph__{std::move(glyph_)},
        meta__{std::move(meta_)}, handler__{nullptr, registry_detach} {
    handler__.reset(registry_attach(*this));
  };

  static atlas_registry_handler__ *registry_attach(atlas &);
  static void registry_detach(atlas_registry_handler__ *);

private:
  atlas_glyph_map__ glyph__;
  atlas_meta_map__ meta__;
  //------- registry
  std::unique_ptr<atlas_registry_handler__,
                  void (*)(atlas_registry_handler__ *)>
      handler__;
};

struct atlas::builder {
  void link_meta(text::glyph::id_t id, text::glyph g) {
    // need
    // key * -- 1 value
    glyph__.map.insert({id, g});
  };

  void set_monospace() { meta__.is_monospace = true; };

  void set_etalon(text::glyph::id_t id) { glyph__.etalon = id; };

  atlas finalize() {
    return {std::move(name), std::move(glyph__), std::move(meta__)};
  };

  builder(std::string name_) : name{name_} {};

private:
  std::string name;
  atlas_glyph_map__ glyph__;
  atlas_meta_map__ meta__;
};

atlas::builder atlas::construct(std::string_view name_) {
  return {std::string{name_}};
};
}; // namespace iuic::text
