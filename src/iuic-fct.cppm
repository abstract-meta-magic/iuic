
module;

#include <cstddef>
#include <cstdint>
#include <deque>
#include <expected>
#include <iostream>
#include <limits>
#include <list>
#include <print>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:computing.tree;
import :base;
import :layout;
import :computing.kernel;
import :policy;

namespace iuic::computing {

// с computing_context
// Так как вычислительное дерево одно на контекст
// в целом его можно сделать пожирнее,
// но исключительно в рамках НУЖНЫХ инструментов
// Reaname to iuic::computing::tree
// iuic::computing::element
// etc

constexpr inline style::decl root_style_decl{};

struct tree final : public kernel_hardware {
public: // base proxy struct's
  struct const_reverse_range_based_for_proxy;
  struct node {
    union {
      request_size area_request;
      struct {
        ui_rect bordered_area;
        ui_rect borderless_area;
      };
      // err_handler
      // text_request
    };
    style::ref style;
  };

  struct info {
    union {
      const frame_layout *frame_layout;
      const text_layout *text_layout;
    };
    element hierarchy;
    uid_t uid;
    z_order_t order;
    policy::hovered hovered_p{policy::hovered::none};
    policy::event event_p;
  };

public: // BIG-VI
  ~tree() = default;
  tree() = default;
  tree(const tree &) = delete;
  tree &operator=(const tree &) = delete;
  tree(tree &&) = delete;
  tree &operator=(tree &&) = delete;

public: // Public Interface
  void reset() noexcept override;

  void add(style::ref, const frame_layout *layout);

  void add(style::ref, const text_layout *layout);

  void up();

  computing::element last() const noexcept;

  computing::element current() const noexcept;

  computing::element at(size_t) const;

  computing::element root() const noexcept;

  size_t size() const noexcept;

  size_t index_at_last() const noexcept;

  size_t current_index() const noexcept;

  const info *current_info() const noexcept;

  info *current_info() noexcept;

  using iterator = std::vector<element>::const_iterator;
  using reverse_iterator = std::vector<element>::const_reverse_iterator;

  iterator begin() const;

  iterator end() const;

  reverse_iterator rbegin() const;

  reverse_iterator rend() const;

  const info *get_info(element) const;

public: // root style pubic interface
  void set_root_size(ui_size);

private: // kernal
  std::expected<ui_rect, int> get_rect(element) const noexcept override;

  std::expected<const style::cref *, int>
      get_style(element) const noexcept override;

  std::vector<request> get_requests(element) const noexcept override;

  std::variant<const frame_layout *, const text_layout *>
      get_layout(element) const noexcept override;

  std::vector<element> get_childs(element) const noexcept override;

  void attach(element, request_size) noexcept override;

  void apply(element, ui_rect bordered) noexcept override;

  void apply(element, ui_rect bordered, ui_rect borderless) noexcept override;

private: // Data
  struct root_t {
    node node{.style = root_style_decl}; // сам контекст
    info info{};
  } root_{};
  std::vector<node> nodes;
  std::vector<info> info;
  std::vector<computing::element> elements;
  // parent\last_brather

  std::stack<size_t> current_{};
  std::stack<std::pair<size_t, size_t>> parent{};
};
}; // namespace iuic::computing
