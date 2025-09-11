
module;

#include <unordered_set>
#include <vector>

export module iuic.core:pseudo_selector;
import :base;

namespace iuic {

struct pseudo_selector {
  virtual ~pseudo_selector() = default;

  virtual bool is_hovered(uid_t uid) const noexcept = 0;

  virtual bool is_active(uid_t uid) const noexcept = 0;

  virtual bool is_focused(uid_t uid) const noexcept = 0;

  virtual void set_active(uid_t uid) noexcept = 0;

  virtual void set_focused(uid_t uid) noexcept = 0;

  virtual void set_hovered(uid_t uid) noexcept = 0;

  virtual void unset_focused(uid_t uid) noexcept = 0;

  virtual void unset_active(uid_t uid) noexcept = 0;

  virtual void unset_focused() noexcept = 0;

  virtual void unset_active() noexcept = 0;

  virtual void unset_hovered(uid_t uid) noexcept = 0;
};

struct pseudo_selector_snapshot {
  std::unordered_set<uid_t> hovered;
  std::unordered_set<uid_t> focused;
  std::unordered_set<uid_t> active;
};

struct advanced_pseudo_selector : pseudo_selector {

  virtual pseudo_selector_snapshot snapshot() const noexcept = 0;

  // other func
};
}; // namespace iuic
