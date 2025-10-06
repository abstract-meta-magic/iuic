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
#include <print>
#include <string_view>
#include <strings.h>
#include <sys/types.h>
#include <type_traits>
#include <variant>

import iuic.core;

namespace iuic::key_map {

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
  iuic::style res{};

  res.shape.min_size = {iuic::upixel_t{120}, iuic::upixel_t{240}};

  res.positioning.margin.top = iuic::upixel_t{20};
  res.positioning.margin.left = iuic::upixel_t{30};

  res.background = iuic::color::css::red{};
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
  iuic::style res{};

  res.shape.max_size = iuic::ui_adaptive_size{};

  return res;
}();

void button(iuic::context::builder &b, std::invocable<> auto &&call,
            const iuic::style &style = iuic::def_style) {
  static constexpr bool uid_seed{true};

  iuic::uid_t uid;

  b.unit.frame(
      [&](auto &b) {
        // just for uid
        b.unit.frame([&](auto &b) { uid = b.uid.make(&uid_seed); },
                     none_style); // hiden style

        // iuic::animation hovered_anim{ ... };
        // begin -- delta -- end

        /* STATE
        // consteval ctor
        static constexp iuic::pseudo_state btn_active{};

        if (b.state.pseudo(uid) == btn_active) {
          ...
        }

        iuic::event::global::key {}; по всей площади
        iuic::event::local::key {};  в рамках площади элемента
        */

        /* text
           auto trk = b.storage.text.persist(uid,"lable");
           b.unit.text(tkr);

           ...
           e.utils.text.try_visit(e.trk,[](iuic::text_buffer& tb) {
             ...
             tb.replace("my");
             ...
           });

           alternative

           создание текста из одного токена
           для коротких строк
           b.unit.text(iuic::text::token{"window"});
        */

        /* UID
        // ввести понятия наследования uid изночально от root
        // ответвления uid
        b.uid.branch(uid);
        */

        /* POLICY


        // включает возможность hovered и закрепляет uid
        b.policy.hovered(iuic::policy::hovered::propagate);
        // включает события и закрепляет uid
        b.policy.event(iuic::policy::event::consume);

        b.event([](auto& r) {
          // ...
          r.key()
          // ...
        });

        // true и продолжить распространение
        b.policy.hovered(e.uid) = iuic::event::hovered::propagate;
        // true и прекратить распространение
        b.policy.hovered(e.uid) = iuic::event::hovered::consume;
        // сбросить предыдущие, установить терущий в true и продолжить
        распространение b.policy.hovered(e.uid) =
        iuic::event::hovered::scope_propagate;
        // сбросить предыдущие, установить терущий в true и прекратить
        распространение b.policy.hovered(e.uid) =
        iuic::event::hovered::scope_consume;
        // сбросить предыдущие, прекратить распространение
        b.policy.hovered(e.uid) = iuic::event::hovered::block;
        */

        /* Z ORDER
        b.order.group(4); // 4:0

        if(builder.state.pseude(uid) == focused) {
          b.order.up(); // 4:0->1
          b.oredr.down(); // 4:1->0
          b.order.set(6); // 4:6
          b.order.top(); // 4:(max)
        }
        */

        // TODO : замена b.selector.* -> b.state.*

        // e.utils.state.hovered(uid) = true;
        // e.utils.state.hovered(uid) == true;

        /* Dynamic style
           if(e.utils.state.hovered(e.uid)) {
              e.uitls.state.hovered(e.uid) = false;
           }


          if(b.state.is_hovered()) {
             замена
             b.style.override(b.style.sheet("button-def:hovered"));
          } else if(b.selector.is_selected()) {
             наследование
             b.style.extend(b.style.sheet("button-def:selected"));
          }
        */

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
                             const iuic::style &style = iuic::def_style) {
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

template <std::invocable<> auto call>
constexpr const auto &static_ref() noexcept {
  static constexpr auto _{call()};
  return _;
}

// import iuic.kit.base;
namespace iuic::kit {
// base kit
struct base {

  static constexpr void button(iuic::context::builder &b) noexcept;

  static constexpr void button(iuic::context::builder &b,
                               std::invocable<> auto call) noexcept;

  static constexpr void lable(std::string text) noexcept;

  static constexpr trk_t text_field() noexcept;

  static constexpr trk_t text_field(std::string) noexcept;

  struct color {
    static constexpr color_t text;

    static constexpr color_t text_hovered;

    static constexpr color_t text_visited;
  };
};

}; // namespace iuic::kit

void test_list(iuic::context::builder &b, auto begin, auto end, auto call) {
  b.frame([&](auto &b) {
    for (; begin != end; ++begin) {
      // this list item wrapper
      b.frame([&]() {
        // здесь создаеться уникальный id для пересечения элементов.
        // this item frame
        b.frame([&]() { call(*begin); });
      });
    };
  });
}

int main() {
  using color = iuic::kit::base::color;
  using namespace iuic;

  color_t c = color::text;

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
      auto &style = static_ref<[]() {
        iuic::style res{};

        res.shape.min_size = {upixel_t{200}, upixel_t{400}};
        res.background = iuic::color::css::lime{80};
        res.positioning.margin = {upixel_t{20}, upixel_t{20}, upixel_t{20},
                                  upixel_t{20}};

        return res;
      }>();
      // frame(create_info,childs_lambda)
      b.unit.frame([](auto &b) {
        b.unit.frame([](auto &b) { b.unit.frame(); }, style);
      });

      // b.text("ok");

      button(b, [&]() { std::println("yo {}", msg); });

      button(b, []() { std::println("my pritty btn"); });

      button(b, []() { std::println("my pritty btn with style"); }, style);
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
      std::visit(
          [renderer, &rect](auto &obj) {
            if constexpr (std::same_as<std::remove_cvref_t<decltype(obj)>,
                                       frame_render_data>) {
              std::visit(
                  [&](auto &&c) {
                    if constexpr (std::same_as<std::remove_cvref_t<decltype(c)>,
                                               color_t>) {
                      SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                      SDL_RenderFillRect(renderer, &rect);
                      // std::println("r:{}g:{}:b{}:a{}", c.r, c.g, c.b, c.a);
                    } else if constexpr (std::same_as<
                                             std::remove_cvref_t<decltype(c)>,
                                             ui_background_image>) {
                      // not impl yet
                    }
                  },
                  obj.background);
            }
          },
          r.data);
    }

    // std::cout << "GO" << std::endl;

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
