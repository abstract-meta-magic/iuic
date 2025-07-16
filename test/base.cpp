

#include "iuic/context"

template <iuic::style style = iuic::def_style> void button(auto &b) {
  b.frame<style>([](auto &b) {
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

  ctx.make([](auto &b) {
    b.frame([](auto &b) { return b.ret(); });

    button(b);
    button<{}>(b); // new style
    button(b);

    return b.ret();
  });

  ctx.event.key("A", KeyAction::Down);
  ctx.event.key("B", KeyAction::Down);
  ctx.event.pointer({22, 44});

  ctx.

      return 0;
}
