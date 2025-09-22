

module;

#include <expected>
#include <variant>
export module iuic.core:layout.def;
import :base;

export namespace iuic {

struct request_size {
  ui_adaptive_unit width;
  ui_adaptive_unit height;
};

struct measure_request {
  request_size rq;
  enum type {
    base,
    // static
    // sticky
    // etc
  };
};

struct frame_measure_utils;
struct frame_arrange_utils;
struct frame_position_utils;

enum class measure_err {
  undefined,
  discarted,
  broken,
};

using measure_result = std::expected<measure_request, measure_err>;

struct frame_layout {
  virtual ~frame_layout() = default;

  virtual measure_result measure(frame_measure_utils) const noexcept = 0;

  virtual void arrange(frame_arrange_utils) const noexcept = 0;

  virtual void position(frame_position_utils) const noexcept = 0;
};

struct text_measure_utils;
struct text_arrange_utils;

struct text_layout {
  virtual ~text_layout() = default;

  virtual void measure(text_measure_utils) const noexcept = 0;

  virtual void arrange(text_arrange_utils) const noexcept = 0;
};

}; // namespace iuic
