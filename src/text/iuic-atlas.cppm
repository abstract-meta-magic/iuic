// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:atlas;
import std;
import iuic.underlying;
import :glyph;

export namespace iuic::text {

struct atlas {
  using id = std::size_t;

  struct : utils::adv_member_for<atlas> {
    const glyph &operator[](glyph::id_t);
  } glyph [[no_unique_address]];

  struct : utils::adv_member_for<atlas> {
    const struct glyph &operator[]();

    void heh() { self().glyph[4]; };

    // позиция в атласе,uv и т.д.
  } meta [[no_unique_address]];

  std::unique_ptr<external::binding> binding;

  static const atlas &by_name(std::string_view name);
  static const atlas &by_symname(std::string_view name);
  static const atlas &by_family(std::string_view name);
  static const atlas &by_id(id id);

  static constexpr id invalid_id{std::numeric_limits<std::size_t>::max()};
  static id get_id(const atlas &);
  id get_id() const;

  atlas() { resistry_attach(*this); };

protected:
  static void resistry_attach(atlas &);
  static void resistry_detach(atlas &);
  ~atlas() { resistry_detach(*this); };
};
}; // namespace iuic::text
