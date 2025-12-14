
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
import :kernel;
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

struct tree final {
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
    kernel::element hierarchy;
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
  void reset() noexcept;

  void add(style::ref, const frame_layout *layout);

  void add(style::ref, const text_layout *layout);

  void up();

  kernel::element last() const noexcept;

  kernel::element current() const noexcept;

  kernel::element at(size_t) const;

  kernel::element root() const noexcept;

  size_t size() const noexcept;

  size_t index_at_last() const noexcept;

  size_t current_index() const noexcept;

  const info *current_info() const noexcept;

  info *current_info() noexcept;

  using iterator = std::vector<kernel::element>::const_iterator;
  using reverse_iterator = std::vector<kernel::element>::const_reverse_iterator;

  iterator begin() const;

  iterator end() const;

  reverse_iterator rbegin() const;

  reverse_iterator rend() const;

  const info *get_info(kernel::element) const;

public: // root style pubic interface
  void set_root_size(ui_size);

private: // kernal
private: // Data
  struct root_t {
    node node{.style = root_style_decl}; // сам контекст
    info info{};
  } root_{};
  std::vector<node> nodes;
  std::vector<info> info;
  std::vector<kernel::element> elements;
  // parent\last_brather

  std::stack<size_t> current_{};
  std::stack<std::pair<size_t, size_t>> parent{};
};
}; // namespace iuic::computing
