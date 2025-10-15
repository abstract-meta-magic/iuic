#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <concepts>
#include <iostream>
#include <memory>
#include <print>
#include <string_view>
#include <strings.h>
#include <sys/types.h>
#include <type_traits>
#include <variant>

import iuic.core;

namespace iuic::key_map {

struct command {
  virtual ~command() = default;
  virtual std::unique_ptr<command> proccess(struct game *) = 0;
  virtual void undo(struct game *) = 0;
};

struct flex_event {
  enum class type {
    sshh,
  } type;
  char data[];
};

constexpr key_code mouse(std::string_view str) {
  if (str == "left") {
    return key_code{(key_t)1};
  } else if (str == "mid") {
    return key_code{(key_t)2};
  } else if (str == "right") {
    return key_code{(key_t)3};
  }
  return {};
};
}; // namespace iuic::key_map

constexpr auto s_1 = []() {
  iuic::style::decl res{};

  res.shape.min_size = {iuic::upixel_t{120}, iuic::upixel_t{240}};

  res.shape.margin.top = iuic::upixel_t{20};
  res.shape.margin.left = iuic::upixel_t{30};

  res.decoration.background = iuic::color::css::red{};
  return res;
}();

SDL_FRect to_sdl_rect(const iuic::ui_rect &val) {
  SDL_FRect res;

  res.h = val.size.h;
  res.w = val.size.w;
  res.x = val.position.x;
  res.y = val.position.y;

  return res;
};

using builder_ui = iuic::context::builder;

constexpr auto none_style = []() {
  iuic::style::decl res{};

  res.shape.max_size = iuic::ui_adaptive_size{};

  return res;
}();

void novel_text_left_menu(iuic::context::builder &b) {
  b.unit.frame([](auto &b) {
    //...
  });
}

enum class type { local, global };

struct my_custom_event {};

iuic::trk_t novel_text(iuic::context::builder &b) {
  iuic::trk_t extern_buff;

  b.unit.frame([&](auto &b) {
    auto uid = b.uid.make("novet-text-box");
    b.policy.hovered(iuic::policy::hovered::block);

    novel_text_left_menu(b);
    static auto text_set = iuic::pseudo_state::make<struct text_set>();
    static auto text_forse_set =
        iuic::pseudo_state::make<struct text_force_set>();
    static auto text_proccess = iuic::pseudo_state::make<struct text_set>();
    static auto text_set_next =
        iuic::pseudo_state::make<struct text_set_next>();

    // extern text
    b.event.attach([](iuic::event::local::key e) {});

    b.event([](iuic::event::local::utils u, my_custom_event e) {
      // ...
    });
    // my buff

    // static text

    // text wrapper
    b.unit.frame([&](auto &b) {
      auto trk = b.storage.text.persist(uid, "text");
      extern_buff = b.storage.text.persist(uid, "extern-text");
      b.unit.text(trk);

      b.policy.hovered(iuic::policy::hovered::propagate);

      b.event([](iuic::event::global::key e) {
        if (e.utils.state.pseudo(e.uid) == text_proccess) {
          e.utils.state.pseudo(e.uid) = text_forse_set;
        } else if (e.utils.state.pseudo(e.uid) == text_set) {
          e.utils.state.pseudo(e.uid) = text_set_next;
        }
      });
    });
  });

  return extern_buff;
}

struct style_ref {};

struct style {
  style_ref background;
};

void button(iuic::context::builder &b, std::invocable<> auto &&call,
            iuic::style::ref style = iuic::def_style) {
  static constexpr bool uid_seed{true};

  iuic::uid_t uid;

  b.unit.frame(
      [&](auto &b) {
        // just for uid
        b.unit.frame([&](auto &b) { uid = b.uid.make(&uid_seed); },
                     none_style); // hiden style

        // iuic::animation hovered_anim{ ... };
        // begin -- delta -- end

        /* Dynamic style

           if(b.state.hovered(uid)) {
             b.style.dynamic(iuic::style::decoratin{.background =
           *::color::red});
           }

           // style property : begind proccess end


           // перед вычисления макета
           b.transition<struct idle,struct load>([](auto u){
             сделать корутины
             ...

             if(u.time.elipce > 1s) {
               ...
               u.state.pseudo = view;
               return tr_continue{};
             }
             ...
             return tr_end{};
           },ork);

           b.transition<struct focused,struct idle>([](...) { ... });
        */

        if (b.state.hovered(uid)) {
          b.style.dynamic(
              iuic::style::decoration{.background = iuic::color::css::red()});
        };

        auto srk = b.storage.object.persist(uid, "callback");
        auto trk = b.storage.text.persist(uid, "lable");

        b.unit.text(trk);

        b.storage.object.init_if_not(srk, [&]() { return call; });

        b.uid.branch(uid); // event set

        b.policy.hovered(iuic::policy::hovered::propagate);

        using callback_t = std::remove_cvref_t<decltype(call)>;
        b.event(
            [](iuic::event::local::key e) {
              if (e.code == iuic::key_map::mouse("left")) {
                e.utils.object.try_visit(e.ork,
                                         [](callback_t &call) { call(); });
              };
            },
            srk);
      },
      style);
}

constexpr inline void button(iuic::context::builder &b, const std::string &str,
                             iuic::style::ref style = iuic::def_style) {
  static constexpr bool uid_sub_seed{true};

  auto uid = b.uid.make(b.uid.make(&uid_sub_seed), str);

  static auto focused = iuic::pseudo_state::make<struct focused>();
  static auto idle = iuic::pseudo_state::make<struct idle>();

  b.unit.frame(
      [=](auto &b) {
        // b.text(str);

        b.uid.branch(uid);
        auto trk = b.storage.text.persist(uid, "haha");

        b.unit.text(trk);

        b.policy.hovered(iuic::policy::hovered::propagate);

        b.event([](iuic::event::local::key e) {
          if (e.code == iuic::key_map::mouse("left")) {
            if (e.utils.state.pseudo(e.uid) != focused) {
              e.utils.state.pseudo(e.uid) = focused;
            }
          }
        });

        b.event([](iuic::event::global::key e) {
          std::println("Text field press : {}", e.code[0]);
          if (e.utils.state.pseudo(e.uid) == focused &&
              e.code == iuic::key_map::mouse("left")) {
            e.utils.state.pseudo(e.uid) = idle;
          }
        });
      },
      style);
}

template <auto object> constexpr const auto &static_ref() noexcept {
  return object;
}

int main() {
  using namespace iuic;

  context ctx;

  // SDL BASE
  SDL_Init(SDL_INIT_VIDEO);
  SDL_InitFlags window_flags = SDL_WINDOW_RESIZABLE |
                               SDL_WINDOW_HIGH_PIXEL_DENSITY |
                               SDL_WINDOW_TRANSPARENT;
  SDL_Window *window{SDL_CreateWindow("IUIC - Test", 600, 800, window_flags)};
  SDL_InitFlags rd = SDL_BLENDMODE_BLEND;
  SDL_Renderer *renderer{SDL_CreateRenderer(window, nullptr)};
  SDL_SetRenderVSync(renderer, 1);
  SDL_SetRenderDrawBlendMode(renderer, true);
  // END

  if (not window) {
    std::cout << "ERR : Window not ceated" << std::endl;
  }
  if (not renderer) {
    std::cout << "ERR : Renderer not ceated" << std::endl;
  }
  SDL_Event e;

  bool quit{false};

  std::string msg{"Hello world"};

  while (!quit) {

    ctx.make([&](auto &b) {
      // frame(create_info,childs_lambda)
      b.unit.frame([&](auto &b) {
        static auto idle = iuic::pseudo_state::make<struct idle>();
        static auto focused = iuic::pseudo_state::make<struct forused>();

        // b.state.pseudo.init_value(idle);

        b.unit.frame([&](auto &b) {
          auto style = []() -> iuic::style::ref {
            static iuic::style::decl res{};
            res.shape.min_size = {iuic::upixel_t{240}, iuic::upixel_t{60}};

            res.decoration.background = iuic::color::css::red();

            return res;
          }();
          b.unit.frame(
              [&](auto &b) {
                auto inner = b.uid.make("app-box-inner");
                b.uid.branch(inner);

                b.state.pseudo_default(inner, idle);

                b.policy.hovered(iuic::policy::hovered::block);

                if (b.state.pseudo(inner) == focused) {
                  b.style.dynamic(iuic::style::decoration{
                      .background = iuic::color_t{44, 22, 99, 255}});
                }

                b.event([](iuic::event::local::key e) {
                  if (e.utils.state.pseudo(e.uid) != focused) {
                    std::println("Set to focuse");
                    if (e.code == iuic::key_map::mouse("left")) {
                      e.utils.state.pseudo(e.uid) = focused;
                    }
                  }
                });

                b.event([](iuic::event::global::key e) {
                  if (e.utils.state.pseudo(e.uid) == focused &&
                      not e.utils.state.hovered(e.uid) &&
                      e.code == iuic::key_map::mouse("left")) {
                    e.utils.state.pseudo(e.uid) = idle;
                  } else if (e.utils.state.pseudo(e.uid) == focused) {
                    std::println("In focuse");
                  };
                });
              },
              style);
          b.uid.branch(b.uid.make("app-box"));
          b.policy.hovered(iuic::policy::hovered::propagate);

          b.event([](iuic::event::local::key e) { std::println("outer"); });
        });
      });

      // b.text("ok");

      button(b, [&]() { std::println("yo {}", msg); });

      button(b, []() { std::println("my pritty btn"); });

      button(b, []() { std::println("my pritty btn with style"); });
    });

    // SDL
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      } else if (e.type == SDL_EVENT_KEY_DOWN) {
        ctx.event.key(iuic::key_code{static_cast<iuic::key_t>(e.key.scancode)});
      } else if (e.type == SDL_EVENT_MOUSE_MOTION) {
        // test pointer event

        ctx.event.pointer_move({(int)e.motion.x, (int)e.motion.y});
      } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        ctx.event.key(key_code{e.button.button});
      };
    }

    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    ctx.set_view_size({static_cast<upixel_t>(w), static_cast<upixel_t>(h)});

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // std::cout << "GO" << std::endl;
    auto &tree = ctx.get_tree();
    for (auto &&r : tree) {
      auto rect = to_sdl_rect(r.area);

      auto &b = r.style.get_decoration().background;
      std::visit(
          [&](auto &obj) {
            using type = std::remove_cvref_t<decltype(obj)>;
            if constexpr (std::same_as<type, iuic::color_t>) {
              SDL_SetRenderDrawColor(renderer, obj.r, obj.g, obj.b, obj.a);
              SDL_RenderFillRect(renderer, &rect);
            }
          },
          b);
    }

    SDL_RenderDebugText(renderer, 0, 0, "penis");

    // std::cout << "GO" << std::endl;

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
