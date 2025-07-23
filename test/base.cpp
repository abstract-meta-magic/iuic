#include <iostream>
import iuic.core;

template <iuic::style style = iuic::def_style> void button(auto &b) {
  b.template frame<style>([](auto &b) {
    b.frame([](auto &b) {
      //
    });
    b.frame([](auto &b) {
      //
    });
    b.frame([](auto &b) {
      //
    });
  });
}

int main() {
  using namespace iuic;

  context ctx;

  ui_size sz{1920, 1080};

  ctx.set_view_size(sz);

  ctx.make([](auto &b) {
    // frame(create_info,childs_lambda)
    b.frame([](auto &b) { b.frame([](auto &b) {}); });

    button(b);
    button(b); // new style
    button(b);
  });

  std::cout << "complite" << std::endl;

  ctx.event.key("A", KeyAction::Down);
  ctx.event.key("B", KeyAction::Down);
  ctx.event.pointer({22, 44});

  return 0;
}
