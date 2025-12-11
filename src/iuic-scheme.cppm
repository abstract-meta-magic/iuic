module;

#include <concepts>
#include <cstddef>
#include <optional>
#include <print>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

export module iuic.core:scheme;
import :base;
import :style;
import :text.present;

namespace iuic {

namespace scheme {

struct element {
  ui_rect rect; // x,y w,h
  std::optional<text::glyph::sequence> text{std::nullopt};
  z_order_t order;
  style::cref style;
  enum class mask {
    Pop = 1 << 0,
  };
};

struct incomplete {

  void push_text(ui_rect rect, text::glyph::sequence text_, style::cref style) {
    elements.push_back(element{.rect = rect, .text = text_, .style = style});
  };

  void push_frame(ui_rect rect, style::cref style) {
    elements.push_back(element{.rect = rect, .style = style});
  };

  std::vector<element> &&extract() && { return std::move(elements); };

  std::vector<element> copy() const & { return elements; };

private:
  std::vector<element> elements;
};

export struct frame {
  ui_rect rect;
  style::cref style;
};

export struct text {
  ui_rect rect;
  iuic::text::glyph::sequence text;
  style::cref style;
};

export struct dump_t {
  // ...
};

template <typename T>
concept frame_visit_cpt =
    requires(T obj, const ui_rect &rect, const style::cref &style) {
      obj.frame(rect, style);
    } || requires(T obj, const ui_rect &rect, const style::cref &style) {
      obj.operator()(rect, style);
    } || requires(T obj, const ui_rect &rect, const style::cref &style) {
      obj.operator()({rect, style});
    } || requires(T obj, const ui_rect &rect, const style::cref &style) {
      obj.frame({rect, style});
    };

template <typename T>
concept text_visit_cpt =
    requires(T obj, const ui_rect &rect, iuic::text::glyph::sequence present,
             const style::cref &style) { obj.text(rect, present, style); } ||
    requires(T obj, const ui_rect &rect, iuic::text::glyph::sequence present,
             const style::cref &style) {
      obj.operator()(rect, present, style);
    } ||
    requires(T obj, const ui_rect &rect, iuic::text::glyph::sequence present,
             const style::cref &style) {
      obj.operator()({rect, present, style});
    } ||
    requires(T obj, const ui_rect &rect, iuic::text::glyph::sequence present,
             const style::cref &style) { obj.text({rect, present, style}); };

// TODO : mb rename to any_*
template <typename T>
concept partial_visit_cpt = frame_visit_cpt<T> != text_visit_cpt<T>;

template <typename T>
concept full_visit_cpt = frame_visit_cpt<T> && text_visit_cpt<T>;

template <typename T>
concept has_visit_base_cpt = partial_visit_cpt<T> || full_visit_cpt<T>;

template <typename T>
concept variant_visit_cpt = requires {
  []<has_visit_base_cpt... Ts>(std::type_identity<std::variant<Ts...>>) {
    static_assert(sizeof...(Ts) > 0);
  }(std::type_identity<std::remove_cvref_t<T>>{});
};

template <typename T>
concept has_visit_cpt =
    partial_visit_cpt<T> || full_visit_cpt<T> || variant_visit_cpt<T>;

template <typename T>
concept tuple_visit_cpt = requires {
  []<has_visit_cpt... Ts>(std::type_identity<std::tuple<Ts...>>) {
    static_assert(sizeof...(Ts) > 0);
  }(std::type_identity<std::remove_cvref_t<T>>{});
};

struct explorer {

  explorer(incomplete &&inc) : elements{std::move(inc).extract()} {};
  explorer(const incomplete &inc) : elements{inc.copy()} {};
  explorer &operator=(incomplete &&inc) {
    elements = std::move(inc).extract();
    return *this;
  };
  explorer &operator=(const incomplete &inc) {
    elements = inc.copy();
    return *this;
  };

  void explore(tuple_visit_cpt auto &&) const;

  void explore(has_visit_cpt auto &&...visitors) const
    requires(sizeof...(visitors) > 0);

  void explore(ui_rect rect, tuple_visit_cpt auto &&) const;

  void explore(ui_rect rect, has_visit_cpt auto &&...visitors) const
    requires(sizeof...(visitors) > 0);

  void explore(const dump_t &d, tuple_visit_cpt auto &&) const;

  void explore(const dump_t &, has_visit_cpt auto &&...visitors) const
    requires(sizeof...(visitors) > 0);

  dump_t dump() const;

private:
  void visit(const element &el, frame_visit_cpt auto &visitor) const;

  void visit(const element &el, text_visit_cpt auto &visitor) const;

  void visit_text_element(const element &el,
                          has_visit_cpt auto &&...visitors) const;

  void visit_frame_element(const element &el,
                           has_visit_cpt auto &&...visitors) const;

  void visit_if_text(const element &el, has_visit_cpt auto &&) const;

  void visit_if_frame(const element &el, has_visit_cpt auto &&) const;

  void resolve_text_visitor(const element &el, variant_visit_cpt auto &&) const;

  void resolve_frame_visitor(const element &el,
                             variant_visit_cpt auto &&) const;

private:
  std::vector<element> elements;
  iuic::hash::hash_t hash;
};

// --- VISIT ---

void explorer::visit(const element &el, frame_visit_cpt auto &visitor) const {
  // do call
  if constexpr (requires() { visitor(el.rect, el.style); }) {
    visitor(el.rect, el.style);
  } else if constexpr (requires() { visitor.frame(el.rect, el.style); }) {
    visitor.frame(el.rect, el.style);
  } else if constexpr (requires() { visitor({el.rect, el.style}); }) {
    visitor({el.rect, el.style});
  } else if constexpr (requires() { visitor.frame({el.rect, el.style}); }) {
    visitor.frame({el.rect, el.style});
  }
};

void explorer::visit(const element &el, text_visit_cpt auto &visitor) const {
  if (!el.text) {
    // err
    return;
  }

  if constexpr (requires() { visitor(el.rect, el.text.value(), el.style); }) {
    visitor(el.rect, el.text.value(), el.style);
  } else if constexpr (requires() {
                         visitor.frame(el.rect, el.text.value(), el.style);
                       }) {
    visitor.frame(el.rect, el.text, el.style);
  } else if constexpr (requires() {
                         visitor({el.rect, el.text.value(), el.style});
                       }) {
    visitor({el.rect, el.text.value(), el.style});
  } else if constexpr (requires() {
                         visitor.frame({el.rect, el.text.value(), el.style});
                       }) {
    visitor.frame({el.rect, el.text.value(), el.style});
  }
};

void explorer::resolve_text_visitor(const element &el,
                                    variant_visit_cpt auto &&variant) const {
  std::visit([&](auto &visitor) { visit_if_text(el, visitor); }, variant);
};

void explorer::resolve_frame_visitor(const element &el,
                                     variant_visit_cpt auto &&variant) const {
  std::visit([&](auto &visitor) { visit_if_frame(el, visitor); }, variant);
};

void explorer::visit_if_text(const element &el,
                             has_visit_cpt auto &&visitor) const {
  if constexpr (text_visit_cpt<decltype(visitor)>) {
    visit(el, visitor);
  } else if constexpr (variant_visit_cpt<decltype(visitor)>) {
    resolve_text_visitor(el, visitor);
  }
};

void explorer::visit_if_frame(const element &el,
                              has_visit_cpt auto &&visitor) const {
  if constexpr (frame_visit_cpt<decltype(visitor)>) {
    visit(el, visitor);
  } else if constexpr (variant_visit_cpt<decltype(visitor)>) {
    resolve_frame_visitor(el, visitor);
  }
};

void explorer::visit_text_element(const element &el,
                                  has_visit_cpt auto &&...visitors) const {
  (visit_if_text(el, visitors), ...);
};

void explorer::visit_frame_element(const element &el,
                                   has_visit_cpt auto &&...visitors) const {
  (visit_if_frame(el, visitors), ...);
};

// --- EXPLORE ---
void explorer::explore(has_visit_cpt auto &&...visitors) const
  requires(sizeof...(visitors) > 0)
{
  for (auto &&element : elements) {
    if (element.text) {
      visit_text_element(element, visitors...);
    } else {
      visit_frame_element(element, visitors...);
    }
  }
}

void explorer::explore(const dump_t &dump,
                       has_visit_cpt auto &&...visitors) const
  requires(sizeof...(visitors) > 0)
{
  // wrong
  for (auto &&element : elements) {
    if (element.text) {
      visit_text_element(element, visitors...);
    } else {
      visit_frame_element(element, visitors...);
    }
  }
}

void explorer::explore(ui_rect rect, has_visit_cpt auto &&...visitors) const
  requires(sizeof...(visitors) > 0)
{
  // code
  for (auto &&element : elements) {
    // do job
  }
}

// --- RECALL ---
void explorer::explore(tuple_visit_cpt auto &&visitor) const {
  std::apply(
      [this](auto &&...visitors) {
        explore(std::forward<decltype(visitors)>(visitors)...);
      },
      std::forward<decltype(visitor)>(visitor));
};

void explorer::explore(const dump_t &dump,
                       tuple_visit_cpt auto &&visitor) const {
  std::apply(
      [this, &dump](auto &&...visitors) {
        explore(dump, std::forward<decltype(visitors)>(visitors)...);
      },
      std::forward<decltype(visitor)>(visitor));
};

void explorer::explore(ui_rect rect, tuple_visit_cpt auto &&visitor) const {
  std::apply(
      [this, rect = std::move(rect)](auto &&...visitors) {
        explore(std::move(rect), std::forward<decltype(visitors)>(visitors)...);
      },
      std::forward<decltype(visitor)>(visitor));
};

} // namespace scheme

}; // namespace iuic
