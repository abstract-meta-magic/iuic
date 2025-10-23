

#include <concepts>
#include <functional>
#include <print>
#include <string>
#include <type_traits>
#include <utility>
import iuic.core;

using namespace iuic;

void button(context::builder &b, std::invocable<> auto &&call) {
  using bt = context::builder;
  int dy_seed{};

  b.unit.frame([&](bt &b) {
    auto uid = b.uid.make(policy::unique{}, "button");

    b.uid.branch(uid);
    b.policy.hovered(policy::hovered::propagate);

    auto ork = b.storage.object.persist(uid, "callback");
    b.storage.object.init_if_not(
        ork, [&]() { return std::forward<decltype(call)>(call); });

    using callback_t = std::remove_cvref_t<decltype(call)>;
    b.event(
        [](event::local::key e) {
          if (e.code == key_code{}) {
            e.utils.object.try_visit(
                e.ork, [](callback_t &call) { std::invoke(call); });
          }
        },
        ork);
  });
}

int main() {
  context ctx;

  ctx.set_view_size({400, 800});

  ctx.make([](auto &b) {
    std::string msg{"He-he you tuch me >~<"};
    button(b, [=]() { std::println("{}", msg); });
  });

  return 0;
}
