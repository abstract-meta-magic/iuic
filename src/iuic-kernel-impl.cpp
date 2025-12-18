module iuic.core;
import std;
import :base;
import :kernel;

namespace iuic::kernel {
namespace {
static constexpr inline std::size_t invalide_index{
    std::numeric_limits<std::size_t>::max()};
}

struct root_layout : frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override {
    auto style = utils.self_style();
    return {{style.get_shape().max_size.w, style.get_shape().max_size.h}};
  };

  std::tuple<upixel_t, upixel_t, upixel_t, upixel_t>
  border_of(const request &rq, frame_arrange_utils &utils) const {
    auto rq_style = utils.style_of(rq);
    auto btop = utils.width_upixel_of(rq_style.get_shape().border.top);
    auto bbottom = utils.width_upixel_of(rq_style.get_shape().border.bottom);
    auto bleft = utils.height_upixel_of(rq_style.get_shape().border.left);
    auto bright = utils.height_upixel_of(rq_style.get_shape().border.right);
    return {btop, bbottom, bleft, bright};
  };

  std::tuple<upixel_t, upixel_t, upixel_t, upixel_t>
  margin_of(const request &rq, frame_arrange_utils &utils) const {
    auto rq_style = utils.style_of(rq);
    auto mtop = utils.width_upixel_of(rq_style.get_shape().margin.top);
    auto mbottom = utils.width_upixel_of(rq_style.get_shape().margin.bottom);
    auto mleft = utils.height_upixel_of(rq_style.get_shape().margin.left);
    auto mright = utils.height_upixel_of(rq_style.get_shape().margin.right);
    return {mtop, mbottom, mleft, mright};
  };

  bool arrange(frame_arrange_utils utils) const noexcept override {
    auto self_area = utils.self_area();
    auto self_style = utils.self_style();

    // TOTO : normal arrange
    int y{0};
    for (auto &rq : utils.get_requests()->range()) {
      // bordered + borderless

      auto [btop, bbottom, bleft, bright] = border_of(rq, utils);
      auto [mtop, mbottom, mleft, mright] = margin_of(rq, utils);

      auto width = utils.width_upixel_of(rq.size.width);
      auto height = utils.height_upixel_of(rq.size.height);

      y += mtop;
      utils.apply(rq, ui_rect{(pixel_t)mleft, y, width, height});
      y += height;
    };
    return true;
  };
} root_layout;

struct base_state_model_impl : state_model {
  void attach(iuic::uid_t uid, iuic::state state) noexcept {
    if (states.contains(uid)) {
      states.at(std::size_t{uid}).insert(state);
    } else {
      states.insert({uid, {state}});
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

struct base_memory_model_impl : memory_model {
  void reserve(iuic::uid_t uid,
               const type *type = type::none()) noexcept override {
    if (swap && not swap_2.contains(uid)) {
      swap_2.insert({uid, {.type = type}});
    } else if (not swap_1.contains(uid)) {
      std::println("obj rq {}", uid);
      swap_1.insert({uid, {.type = type}});
    }
  };

  void *locate(iuic::uid_t uid, const type *type) noexcept override {
    if (swap && swap_2.contains(uid)) {
      return swap_2.at(uid).data;
    } else if (swap_1.contains(uid)) {
      return swap_1.at(uid).data;
    } else {
      return nullptr;
    }
  };

  object_state state(iuic::uid_t uid,
                     const type *type = type::none()) const noexcept override {
    // impl
    if (swap) {
      if (swap_2.contains(uid)) {
        auto &obj = swap_2.at(uid);

        if (obj.type == type::none()) {
          return object_state::reserve_none_type;
        }

        if (obj.alive) {
          return obj.type == type ? object_state::alive_this_type
                                  : object_state::alive_other_type;
        } else {
          return obj.type == type ? object_state::reserve_this_type
                                  : object_state::reserve_other_type;
        }
      } else {
        return object_state::none_exist;
      };
    } else {
      if (swap_1.contains(uid)) {
        auto &obj = swap_1.at(uid);

        if (obj.type == type::none()) {
          return object_state::reserve_none_type;
        }

        if (obj.alive) {
          return obj.type == type ? object_state::alive_this_type
                                  : object_state::alive_other_type;
        } else {
          return obj.type == type ? object_state::reserve_this_type
                                  : object_state::reserve_other_type;
        }
      } else {
        return object_state::none_exist;
      };
    }
    return memory_model::object_state::alive_other_type;
  };

  bool update_livetime(iuic::uid_t uid) const noexcept override {
    if (swap && swap_1.contains(uid) && swap_1.at(uid).alive) {
      swap_2.insert(swap_1.extract(uid));
    } else if (swap_2.contains(uid) && swap_2.at(uid).alive) {
      swap_1.insert(swap_2.extract(uid));
    } else {
      return false;
    }
    return true;
  };

  void launch(iuic::uid_t uid, const type *type) noexcept override {
    if (type == type::none()) {
      return;
    }

    // allocate memory
    if (swap && swap_2.contains(uid) && not swap_2.at(uid).alive) {
      auto &obj = swap_2.at(uid);
      if (obj.type == type) {
        if (auto *mem = persist.allocate(type->size, type->align)) {
          obj.data = mem;
          obj.alive = true;
        }
      } else if (obj.type == type::none()) {
        if (auto *mem = persist.allocate(type->size, type->align)) {
          obj.data = mem;
          obj.type = type;
          obj.alive = true;
        }
      }
    } else if (swap_1.contains(uid) && not swap_1.at(uid).alive) {
      std::println("try init");
      auto &obj = swap_1.at(uid);
      if (obj.type == type) {
        if (auto *mem = persist.allocate(type->size, type->align)) {
          obj.data = mem;
          obj.alive = true;
        }
      } else if (obj.type == type::none()) {
        if (auto *mem = persist.allocate(type->size, type->align)) {
          std::println("none init");
          obj.data = mem;
          obj.type = type;
          obj.alive = true;
        }
      }
    }
  };

  bool as(iuic::uid_t uid, const type *type) const noexcept override {
    if (swap && swap_2.contains(uid)) {
      return swap_2.at(uid).type == type;
    } else if (swap_1.contains(uid)) {
      return swap_1.at(uid).type == type;
    }
    return false;
  };

  void *tmp(const type *type, std::size_t count) noexcept override {
    return count > 0 ? tmpr.allocate(type->size * count, type->align) : nullptr;
  };

  struct object {
    void *data;
    const type *type;
    bool alive{false};
  };

  mutable std::unordered_map<iuic::uid_t, object> swap_1;
  mutable std::unordered_map<iuic::uid_t, object> swap_2;
  bool swap{false};

  // buffer for 1M ?
  std::array<std::byte, 1024 * 512> rtmpmemory;
  std::pmr::monotonic_buffer_resource tmpr{&rtmpmemory, rtmpmemory.size()};
  std::pmr::unsynchronized_pool_resource persist{};
};

struct simple_kernel : hardware {
  struct node {
    element el;
    style::ref st;
    iuic::uid_t uid;
    union {
      const text_layout *tlayout;
      const frame_layout *flayout;
    };
    //
    request rq;
    ui_rect bordered_rect;
    ui_rect borderless_rect;
    policy::hovered hp;
    policy::event ep;
    z_order_t order;
  };

  std::expected<ui_rect, int>
  get_rect_bordered(element el) const noexcept override {
    if (el.meta && element::root) {
      return root.bordered_rect;
    } else if (auto &eqel = els[el.self].el.meta;
               eqel && (element::arrange | element::alive)) {
      return els[el.self].bordered_rect;
    } else {
      return std::unexpected{2};
    };
  };

  virtual std::expected<ui_rect, int>
  get_rect_borderless(element el) const noexcept override {
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
  get_requests(element el, bool reverse = false) const noexcept override {
    struct rq_iterator : virtual_iterator<const request> {
      constexpr void next() noexcept override {
        if (valid()) {
          for (;;) {
            auto &snode = kernel.els[index];
            auto &bnode = kernel.els[snode.el.brother];

            if (snode.el.brother == snode.el.parent) {
              index = invalide_index;
              break;
            } else if (bnode.el.meta && element::request) {
              index = bnode.el.self;
              break;
            }
            index = bnode.el.self;
          };
        }
      };

      constexpr void prev() noexcept override { index = invalide_index; };

      constexpr bool valid() const noexcept override {
        return index != invalide_index;
      };

      constexpr const request *get() noexcept override {
        if (valid()) {
          auto &node = kernel.els[index];
          return &node.rq;
        } else {
          return nullptr;
        }
      };

      const simple_kernel &kernel;
      std::size_t index;
      rq_iterator(const simple_kernel &kernel_, std::size_t index_)
          : kernel{kernel_}, index{index_} {};
    };
    //
    struct rq_root_iterator : virtual_iterator<const request> {
      constexpr void next() noexcept override {
        if (valid()) {
          ++index;
          for (; index < kernel.els.size(); ++index) {
            if (kernel.els[index].el.meta && element::root_child) {
              return;
            }
          };
          index = invalide_index;
        }
      };

      constexpr void prev() noexcept override { index = invalide_index; };

      constexpr bool valid() const noexcept override {
        return index != invalide_index;
      };

      constexpr const request *get() noexcept override {
        if (valid()) {
          auto &node = kernel.els[index];
          return &node.rq;
        } else {
          return nullptr;
        }
      };

      const simple_kernel &kernel;
      std::size_t index;
      rq_root_iterator(const simple_kernel &kernel_)
          : kernel{kernel_}, index{0} {};
    };

    //

    if (els.size() > el.self + 1 && els[el.self].el.meta && element::request) {
      if (el.meta && element::root) {
        return std::make_unique<rq_root_iterator>(*this);
      } else if (reverse) {
        return invalid_virtual_iterator{};
      } else {
        return els[el.self + 1].el.parent == el.self
                   ? std::unique_ptr<virtual_iterator<const request>>(
                         new rq_iterator{*this, (std::size_t)el.self + 1})
                   : invalid_virtual_iterator{};
      }
    } else {
      return invalid_virtual_iterator{};
    }
  };

  std::variant<const frame_layout *, const text_layout *>
  get_layout(element el) const noexcept override {
    if (el.meta && element::root) {
      return root.flayout;
    } else if (el.meta & element::text) {
      return els.size() > el.self ? els[el.self].tlayout : nullptr;
    } else {
      return els.size() > el.self ? els[el.self].flayout : nullptr;
    }
  };

  element get_parent(element el) override {
    if (el.meta && element::root || el.meta && element::root_child) {
      return element{.meta = element::root};
    } else {
      return els.size() > el.parent ? els[el.parent].el : element{};
    }
  };

  std::unique_ptr<virtual_iterator<const element>>
  get_childs(element el, bool reverse = false) const noexcept override {
    struct ch_iterator : virtual_iterator<const element> {
      void next() noexcept override {
        if (valid() &&
            kernel.els[index].el.brother != kernel.els[index].el.parent) {
          if (kernel.els[index].el.meta && element::root_child) {
            for (auto i{index + 1}; index < kernel.els.size(); ++i) {
              if (kernel.els[i].el.meta && element::root_child) {
                index = i;
                return;
              }
            }
            index = invalide_index;
          } else {
            index = kernel.els[index].el.brother;
          }
        } else {
          index = invalide_index;
        }
      };

      void prev() noexcept override { index = invalide_index; };

      bool valid() const noexcept override { return index == invalide_index; };

      const element *get() noexcept override {
        return valid() ? &kernel.els[index].el : nullptr;
      };

      const simple_kernel &kernel;
      std::size_t index;
      ch_iterator(const simple_kernel &kernel_, std::size_t index_)
          : kernel{kernel_}, index{index_} {}
    };

    struct ch_riterator : virtual_iterator<const element> {
      void next() noexcept override { cindex = invalide_index; };

      void prev() noexcept override { cindex = invalide_index; };

      bool valid() const noexcept override { return cindex == invalide_index; };

      const element *get() noexcept override {
        return valid() ? &kernel.els[cindex].el : nullptr;
      };

      std::size_t get_last(std::size_t in) {
        for (; kernel.els[in].el.brother == kernel.els[in].el.parent;) {
          in = kernel.els[in].el.brother;
        }
        return in;
      };

      const simple_kernel &kernel;
      std::size_t bindex;
      std::size_t cindex;

      ch_riterator(const simple_kernel &kernel_, std::size_t index_)
          : kernel{kernel_}, bindex{index_}, cindex{get_last(index_)} {}
    };

    if (els.empty()) {
      return invalid_virtual_iterator{};
    }

    if (el.meta && element::root) {
      if (reverse) {
        return not els.empty()
                   ? std::unique_ptr<
                         virtual_iterator<const element>>{new ch_riterator{
                         *this, 0}}
                   : invalid_virtual_iterator{};
      } else {
        return not els.empty()
                   ? std::unique_ptr<
                         virtual_iterator<const element>>{new ch_iterator{*this,
                                                                          0}}
                   : invalid_virtual_iterator{};
      }
    } else {
      if (reverse) {
        return els.size() > el.self + 1 && els[el.self + 1].el.parent == el.self
                   ? std::unique_ptr<
                         virtual_iterator<const element>>{new ch_riterator{
                         *this, std::size_t{el.self} + 1}}
                   : invalid_virtual_iterator{};
      } else {
        return els.size() > el.self + 1 && els[el.self + 1].el.parent == el.self
                   ? std::unique_ptr<
                         virtual_iterator<const element>>{new ch_iterator{
                         *this, std::size_t{el.self} + 1}}
                   : invalid_virtual_iterator{};
      }
    }

    return invalid_virtual_iterator{};
  };

  std::expected<const style::cref *, int>
  get_style(element el) const noexcept override {
    if (el.self < els.size()) {
      if (el.meta && element::root) {
        return &root.st;
      } else {
        if (els.size() > el.self) {
          return &els[el.self].st;
        } else {
          return std::unexpected{0};
        }
      };
    } else {
      return std::unexpected{0};
    }
  };

  std::expected<z_order_t, int> get_zorder(element el) const noexcept override {
    return {};
  };

  std::expected<iuic::uid_t, int> get_uid(element el) const noexcept override {
    if (el.meta && element::root) {
      return root.uid;
    } else {
      if (els.size() > el.self) {
        return els[el.self].uid;
      } else {

        std::println("Element : self:{} , parent:{} , brother:{} , type:{}",
                     el.self, el.parent, el.brother,
                     el.meta && element::root ? "root" : "none");
        return std::unexpected{0};
      };
    }
  };

  std::expected<policy::hovered, int>
  get_hovered_policy(element el) const noexcept override {
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
  get_event_policy(element el) const noexcept override {
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
  get_elements(bool reverse = false) const override {
    struct el_iterator : virtual_iterator<const element> {
      constexpr void next() noexcept override { ++index; };

      constexpr void prev() noexcept override { index = invalide_index; };

      constexpr bool valid() const noexcept override {
        return kernel.els.size() > index;
      };

      constexpr const element *get() noexcept override {
        return valid() ? &kernel.els[index].el : nullptr;
      };

      el_iterator(const simple_kernel &kernel_, std::size_t index_)
          : kernel{kernel_}, index{index_} {}
      const simple_kernel &kernel;
      std::size_t index;
    };

    struct el_riterator : virtual_iterator<const element> {
      constexpr void next() noexcept override {
        if (index == 0) {
          index = invalide_index;
        } else {
          --index;
        }
      };

      constexpr void prev() noexcept override { index = invalide_index; };

      constexpr bool valid() const noexcept override {
        return index != invalide_index;
      };

      constexpr const element *get() noexcept override {
        return valid() ? &kernel.els[index].el : nullptr;
      };

      el_riterator(const simple_kernel &kernel_, std::size_t index_)
          : kernel{kernel_}, index{index_} {}
      const simple_kernel &kernel;
      std::size_t index;
    };

    if (reverse) {
      return std::make_unique<el_riterator>(*this, els.size() - 1);
    } else {
      return std::make_unique<el_iterator>(*this, 0);
    }

    return invalid_virtual_iterator{};
  };

  // if nothin selected return null element
  element get_selected() const noexcept override { return sel; };

  // STATE
  state_model *state() override { return &st; };

  virtual const state_model *state() const override { return &st; };

  virtual std::uint64_t hash(std::span<const std::byte> h) const override {
    // makeo
    static std::hash<std::string_view> hash{};

    return hash(std::string_view{(const char *)&h[0], h.size()});
  };

  // create and select new element
  element instance(iuic::uid_t uid, const frame_layout *layout,
                   style::ref ref) noexcept override {

    els.push_back({.st = ref, .uid = uid, .flayout = layout});
    auto &node = els.back();

    element el;
    if (sel.meta && element::root) {
      el.self = el.brother = el.parent = els.size() - 1;
      el.meta |= element::root_child;
      sel.meta = sel.meta ^ element::root;
    } else {
      el.self = els.size() - 1;
      el.parent = el.brother = sel.self;
    }

    node.el = el;
    node.rq.element = el;
    sel.self = el.self;
    return el;
  };

  // create and select new element
  element instance(iuic::uid_t uid, const text_layout *layout,
                   style::ref ref) noexcept override {
    els.push_back({.st = ref, .uid = uid, .tlayout = layout});
    auto &node = els.back();

    element el;

    if ((sel.meta & element::root) == element::root) {
      el.self = el.brother = el.parent = els.size() - 1;
      el.meta |= element::root_child;
      el.meta |= element::text;
      sel.meta = sel.meta ^ element::root;
    } else {
      el.self = els.size() - 1;
      el.meta |= element::text;
      el.parent = el.brother = sel.self;
    }

    node.el = el;
    node.rq.element = el;
    sel.self = el.self;

    return el;
  };

  // make element alive and select parent
  element launch(element el) noexcept override {
    auto &node = els[el.self];
    if (node.el.meta && element::root_child) {
      sel.meta |= element::root;
      sel.self = el.parent;
    } else {
      sel.self = el.parent;
    }
    node.el.meta |= element::alive;
    return node.el;
  };

  element discard(element el) noexcept override {
    if (el.self < els.size()) {
      els[el.self].el.meta |= element::discarded;
      return els[el.self].el;
    }
    return {};
  };

  bool validate() const override { return true; };

  void override(element el, style::decoration *d) noexcept override {
    els[el.self].st.override(d);
  };

  void override(element el, style::shape *sh) noexcept override {
    if (el.meta && element::root) {
      root_style.shape = *sh;
    } else {
      els[el.self].st.override(sh);
    }
  };

  void override(element el, style::transform *tr) noexcept override {
    els[el.self].st.override(tr);
  };

  void override(element el, z_order_t o) noexcept override {
    els[el.self].order = o;
  };

  void override(element el, policy::hovered h) noexcept override {
    els[el.self].hp = h;
  };

  void override(element el, policy::event e) noexcept override {
    els[el.self].ep = e;
  };

  memory_model *memory() override { return &mem; };

  const memory_model *memory() const override { return &mem; };

  void attach(element el, request_size rq) noexcept override {
    static std::size_t t{0};
    if (el.self < els.size()) {
      if (not(els[el.self].el.meta && element::request)) {
        els[el.self].rq.size = rq;
        els[el.self].el.meta |= element::request;
      }
    }
  };

  void apply(element el, ui_rect bordered) noexcept override {
    if (el.self > els.size())
      return;
    auto &node = els[el.self];
    if (el.meta && element::root) {
      root.bordered_rect = root.borderless_rect = bordered;
    } else if ((node.el.meta && element::request) &&
               not(node.el.meta && element::arrange)) {
      node.bordered_rect = node.borderless_rect = bordered;
      node.el.meta |= element::arrange;
      auto [x, y, w, h] = bordered.xywh();
    }
  };

  void apply(element el, ui_rect bordered,
             ui_rect borderless) noexcept override {
    auto node = els[el.self];
    if (el.meta && element::root) {
      root.bordered_rect = root.borderless_rect = bordered;
    } else if ((node.el.meta && element::request) &&
               not(node.el.meta && element::arrange)) {
      node.bordered_rect = bordered;
      node.borderless_rect = borderless;
      node.el.meta |= element::arrange;
    }
  };

  void advance() noexcept override {
    els.clear();
    sel.self = std::numeric_limits<std::uint16_t>::max();
    mem.tmpr.release();
    //   mem.swap = !mem.swap;
  };

  style::decl root_style{};

  node root{
      .el = {.meta = element::root},
      .st = root_style,
      .uid = hash(std::as_bytes(std::span("root-uid-seed-o[{}&[{}&[[222"))),
      .flayout = &root_layout,
  };
  std::vector<node> els;
  element sel{.meta = element::root};

  base_memory_model_impl mem{};
  base_state_model_impl st{};

  simple_kernel() = default;
};

std::unique_ptr<hardware> default_kernel() noexcept {
  return std::make_unique<simple_kernel>();
};
}; // namespace iuic::kernel
