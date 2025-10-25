// example

#include <functional>
#include <print>
#include <string>
#include <type_traits>
import iuic.core;

using namespace iuic;

// mb in namespace checkbox_state
static auto idle = pseudo_state::make<struct idle>();
static auto block = pseudo_state::make<struct block>();
static auto set = pseudo_state::make<struct set>();

iuic::uid_t checkbox(context::builder &b, const std::string &name) {

  int dy_seed;

  iuic::uid_t box_state_uid;

  auto uid = b.uid.make(iuic::policy::unique{}, "check-frame");
  b.element.frame([&](auto &b) {
    auto trk = b.storage.text.persist(uid, "check-text");

    b.element.text(trk);

    box_state_uid = b.uid.make(iuic::policy::shared{}, "check-box");
    b.element.frame(box_state_uid, [&](auto &b) {
      b.policy.hovered(policy::hovered::propagate);

      // b.state.pseudo.init(idle);

      b.event([](event::local::key e) {
        if (e.utils.state.pseudo(e.uid) == idle) {
          e.utils.state.pseudo(e.uid) = set;
        } else if (e.utils.state.pseudo(e.uid) == set) {
          e.utils.state.pseudo(e.uid) = idle;
        }
      });
    });
  });

  return box_state_uid;
}

void checkbox(context::builder &b, const std::string &name,
              std::invocable<pseudo_state> auto &&callback) {

  int dy_seed;

  iuic::uid_t box_state_uid;

  auto uid = b.uid.make(iuic::policy::unique{}, "check-frame");
  b.element.frame(uid, [&](auto &b) {
    auto trk = b.storage.text.persist(uid, "check-text");

    b.element.text(trk);

    box_state_uid = b.uid.make(iuic::policy::shared{}, "check-box");
    b.element.frame(box_state_uid, [&](auto &b) {
      b.state.pseudo_init_value(box_state_uid, idle);

      b.policy.hovered(policy::hovered::propagate);

      // b.state.pseudo.init(idle);
      auto ork = b.storage.object.persist(box_state_uid, "callback");
      b.storage.object.init_if_not(
          ork, [&]() { return std::forward<decltype(callback)>(callback); });

      using callback_t = std::remove_cvref_t<decltype(callback)>;

      b.event(
          [](event::local::key e) {
            if (e.utils.state.pseudo(e.uid) == idle) {
              e.utils.state.pseudo(e.uid) = set;

              e.utils.object.try_visit(e.ork, [&](callback_t &call) {
                std::invoke(call, e.utils.state.pseudo(e.uid));
              });
            } else if (e.utils.state.pseudo(e.uid) == set) {
              e.utils.state.pseudo(e.uid) = idle;

              e.utils.object.try_visit(e.ork, [&](callback_t &call) {
                std::invoke(call, e.utils.state.pseudo(e.uid));
              });
            }
          },
          ork);
    });
  });
}

int main() {
  context ctx;

  ctx.set_view_size({44, 477});

  ctx.make([](auto &b) {
    auto ch = checkbox(b, "in love");

    if (b.state.pseudo(ch) == set) {
      // do if you in love
    }

    checkbox(b, "horny", [](pseudo_state state) {
      if (state == set) {
        std::println("yey i'm horny");
      } else {
        std::println("... sed");
      };
    });
  });
};
