module;

#include <optional>
#include <variant>

export module iuic.core:relement;
import :base;
import :text.present;

export namespace iuic {

struct relement {
  ui_rect rect;
  std::optional<const text::present *> text;
  z_order_t order;
  style::cref style;
};

// TODO
struct scheme {
  struct element;
};

struct scheme::element {
  ui_rect area; // x,y w,h
  z_order_t order;
  style::cref style;
};

}; // namespace iuic
