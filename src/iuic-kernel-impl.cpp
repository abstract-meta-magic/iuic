module iuic.core;
import std;
import :base;
import :kernel;

namespace iuic::kernel {

struct state : state_model {
  void attach(iuic::uid_t uid, iuic::state state) noexcept {
    if (states.contains(uid)) {
      states.at(std::size_t{uid}).insert(state);
    }
  };

  void detach(iuic::uid_t uid, iuic::state state) noexcept {
    if (states.contains(uid)) {
      states[uid].erase(state);
    }
  };

  // replace to std::ranges::view
  std::unique_ptr<virtual_iterator<const iuic::state>> get(iuic::uid_t,
                                                           iuic::state) const {
    return invalid_virtual_iterator{};
  };

  // TODE FRAME MODEL
  bool is_exist(iuic::uid_t uid) const { return states.contains(uid); };

  // TODE FRAME MODEL
  bool update_livetime(iuic::uid_t) const { return true; };

  bool has(iuic::uid_t uid, iuic::state state) const {
    return states.contains(uid) ? states.at(uid).contains(state) : false;
  };

  std::unordered_map<iuic::uid_t, std::unordered_set<iuic::state>> states;
};

struct memory : memory_model {
  void reserve(iuic::uid_t, const type * = type::none()) noexcept override {};

  void *locate(iuic::uid_t, const type *) noexcept override { return nullptr; };

  object_state state(iuic::uid_t,
                     const type * = type::none()) const noexcept override {

    return memory_model::object_state::alive_other_type;
  };

  bool update_livetime(iuic::uid_t) const noexcept override { return false; };

  void launch(iuic::uid_t, const type *) noexcept override {};

  bool as(iuic::uid_t, const type *) const noexcept override { return false; };

  void *tmp(const type *type, std::size_t count) noexcept override {
    return count > 0 ? tmpr.allocate(type->size * count, type->align) : nullptr;
  };

  // buffer for 1M ?
  std::array<std::byte, 1024 * 1024 * 1> rmemory;
  std::array<std::byte, 1024 * 512> rtmpmemory;
  std::pmr::monotonic_buffer_resource tmpr{&rtmpmemory, rtmpmemory.size()};
};

struct default_kernel : hardware {
  struct node {
    element el;
    style::ref st;
    iuic::uid_t uid;
    union {
      text_layout *tlayout;
      frame_layout *flayout;
    };
    //
    kernel::request rq;
    ui_rect bordered_rect;
    ui_rect borderless_rect;
    policy::hovered hp;
    policy::event ep;
    z_order_t order;
  };

  node root;
  std::vector<node> els;
  element sel;

  std::expected<ui_rect, int> get_rect_bordered(element el) const noexcept {
    if (el.meta && element::root) {
      return root.bordered_rect;
    } else if (els.size() > el.self &&
               (els[el.self].el.meta && (element::arrange | element::alive))) {
      return els[el.self].bordered_rect;
    } else {
      return std::unexpected{2};
    };
  };

  virtual std::expected<ui_rect, int>
  get_rect_borderless(element el) const noexcept {
    if (el.meta && element::root) {
      return root.bordered_rect;
    } else if (els.size() > el.self &&
               (els[el.self].el.meta && (element::arrange | element::alive))) {
      return els[el.self].bordered_rect;
    } else {
      return std::unexpected{2};
    };
  };

  std::unique_ptr<virtual_iterator<const request>>
  get_requests(element el, bool reverse = false) const noexcept {
    struct rq_iterator : virtual_iterator<const request> {
      void next() noexcept override {};
      void prev() noexcept override {};
      bool valid() const noexcept override { return false; };
      const request *get() noexcept override { return nullptr; };
      const std::vector<node> &els;
      element el;
    };

    struct rq_reverse_iterator : virtual_iterator<const request> {
      void next() noexcept override {};
      void prev() noexcept override {};
      bool valid() const noexcept override { return false; };
      const request *get() noexcept override { return nullptr; };

      std::span<const int> sp{};

      const std::vector<node> &els;
      element el;
    };

    if (not el.meta && element::request) {
      return invalid_virtual_iterator{};
    } else if (reverse) {
      return invalid_virtual_iterator{}; // return reverse
    } else {
      return invalid_virtual_iterator{}; // return base
    };
  };

  std::variant<const frame_layout *, const text_layout *>
  get_layout(element el) const noexcept {
    if (el.meta && element::root) {
      return root.flayout;
    } else if (el.meta && (element::text)) {
      return els.size() > el.self ? els[el.self].tlayout : nullptr;
    } else {
      return els.size() > el.self ? els[el.self].flayout : nullptr;
    }
  };

  element get_parent(element el) {
    if (el.meta && element::root || el.meta && element::root_child) {
      return element{.meta = element::root};
    } else {
      return els.size() > el.parent ? els[el.parent].el : element{};
    }
  };

  std::unique_ptr<virtual_iterator<const element>>
  get_childs(element el, bool reverse = false) const noexcept {
    if (el.meta && element::root) {
      return invalid_virtual_iterator{};
    } else {
      return invalid_virtual_iterator{};
    }
  };

  std::expected<const style::cref *, int> get_style(element el) const noexcept {
    if (el.meta && element::root) {
      return &root.st;
    } else {
      if (els.size() > el.self) {
        return &els[el.self].st;
      } else {
        return std::unexpected{0};
      }
    };
  };

  std::expected<z_order_t, int> get_zorder(element el) const noexcept {
    return {};
  };

  std::expected<iuic::uid_t, int> get_uid(element el) const noexcept {
    if (el.meta && element::root) {
      return root.uid;
    } else {
      if (els.size() > el.self) {
        return els[el.self].uid;
      } else {
        return std::unexpected{0};
      };
    }
  };

  std::expected<policy::hovered, int>
  get_hovered_policy(element el) const noexcept {
    if (el.meta && element::root) {
      return root.hp;
    } else {
      if (els.size() > el.self) {
        return els[el.self].hp;
      } else {
        return std::unexpected{0};
      }
    }
  };

  std::expected<policy::event, int>
  get_event_policy(element el) const noexcept {
    if (el.meta && element::root) {
      return root.ep;
    } else {
      if (els.size() > el.self) {
        return els[el.self].ep;
      } else {
        return std::unexpected{0};
      }
    }
  };

  std::unique_ptr<virtual_iterator<const element>>
  get_elements(bool reverse = false) const {
    return invalid_virtual_iterator{};
  };

  // if nothin selected return null element
  element get_selected() const noexcept { return sel; };

  // STATE
  virtual state_model *state() = 0;

  virtual const state_model *state() const = 0;

  virtual std::uint64_t hash(std::span<const std::byte>) const = 0;

  // create and select new element
  virtual element instance(iuic::uid_t, const frame_layout *,
                           style::ref) noexcept = 0;

  // create and select new element
  virtual element instance(iuic::uid_t, const text_layout *,
                           style::ref) noexcept = 0;

  // make element alive and select parent
  virtual element launch(element) noexcept = 0;

  virtual element discard(element) noexcept = 0;

  virtual bool validate() const = 0;

  virtual void override(element, style::decoration *) noexcept = 0;

  virtual void override(element, style::shape *) noexcept = 0;

  virtual void override(element, style::transform *) noexcept = 0;

  virtual void override(element, z_order_t) noexcept = 0;

  virtual void override(element, policy::hovered) noexcept = 0;

  virtual void override(element, policy::event) noexcept = 0;

  virtual memory_model *memory() = 0;

  virtual const memory_model *memory() const = 0;

  virtual void attach(element, request_size) noexcept = 0;

  virtual void apply(element, ui_rect bordered) noexcept = 0;

  virtual void apply(element, ui_rect bordered,
                     ui_rect borderless) noexcept = 0;

  virtual void advance() noexcept = 0;

  struct memory mem;
  struct state st;
};

std::unique_ptr<hardware> default_kernel() noexcept { return nullptr; };
}; // namespace iuic::kernel
