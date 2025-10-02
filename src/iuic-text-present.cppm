

module;

#include <map>
#include <string_view>
#include <vector>

export module iuic.core:text.present;
import :base;
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

  // ???
  void get_present(uid_t);

private:
  std::map<uid_t, present> presents;
};
}; // namespace iuic::text
