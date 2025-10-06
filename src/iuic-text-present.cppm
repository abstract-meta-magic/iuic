

module;

#include <map>
#include <string_view>
#include <vector>

export module iuic.core:text.present;
import :base;
import :storage.def;
import :text.token;

export namespace iuic::text {

struct present_node : token {
  ui_rect rect;
  //..
};

struct present {
  ui_rect rect;
  std::vector<present_node> nodes;
  // ..
};

}; // namespace iuic::text

namespace iuic::text {

class text_present_aggregator {
public:
  void attach_present(trk_t trk, size_t id) {
    if (presents.contains(id)) {
      presents[id].second = trk;
    } else {
      // make new
      presents.insert({id, {{}, trk}});
    };
  };

  std::pair<present, trk_t> *get_present(size_t id) {
    if (presents.contains(id)) {
      return &presents[id];
    } else {
      return nullptr;
    };
  };

private:
  std::map<size_t, std::pair<present, trk_t>> presents;
};
}; // namespace iuic::text
