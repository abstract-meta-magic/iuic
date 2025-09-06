#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <print>
#include <string_view>
#include <strings.h>
#include <type_traits>
#include <variant>
#include <vector>

import iuic.core;

constexpr auto s_1 = []() {
  iuic::style res{};

  res.shape.min_size = {120, 240};

  res.shape.margin.top = 20;
  res.shape.margin.left = 30;

  res.background.color = {33, 176, 57, 255};

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

struct uuid {
  // в строителе
  // берется текущая глубина
  // и уникальноей имя\hesh
  // можно делать автоматически у
  // элементов с собитиями
  // В ОБЩЕМ
  // у builder нужно сделать метод get_uuid();
};

using builder_ui = iuic::context::builder;

void my_test(builder_ui &b) {
  b.frame([](builder_ui &b) {
    auto uid = b.make_uid(b.make_uid(&my_test), "suu");
  });
};

void game_view_instance();

/*

  msg[nil] My penis is big;

  image[show] lower;

*/

void novel_text(builder_ui &b) {
  auto uid_b = b.make_uid(&novel_text);

  b.frame([](auto &b) {
    // left side bar
    // load\save
    b.frame();

    // middle text
    b.frame();

    // right side bar
    b.frame();
  });
};

void menu(builder_ui &b) {};

void game(builder_ui &b) {};

void save() {};

void gallery(builder_ui &b);

void button(iuic::context::builder &b,
            std::invocable<iuic::event_type::key_h> auto call,
            const iuic::style &style = iuic::def_style) {
  static constexpr bool uid_seed{true};

  iuic::uid_t uid;
  b.frame([&](auto &b) {
    // unique id
    b.frame(
        [&](auto &b) {
          uid = b.make_uid(b.make_uid(&uid_seed));
          b.apply_uid(uid);
          b.event(std::forward<decltype(call)>(call));
        },
        style);

    b.apply_uid(uid);

    // to pseudo-classes :hover, use paren ui_rect
    b.event([](iuic::event_type::pointer_enter e) {
      e.selector.set_hovered(e.uid);
    });
  });
}

constexpr inline void button(iuic::context::builder &b, const std::string &str,
                             const iuic::style &style = iuic::def_style) {
  static constexpr bool uid_sub_seed{true};

  auto uid = b.make_uid(b.make_uid(&uid_sub_seed), str);

  b.frame(
      [=](auto &b) {
        b.text(str);
        if (auto ref = b.storage.get_ref(int{2}); ref.template as<int>()) {
          auto &obj = ref.template unwrap<int>();
        };

        b.apply_uid(uid);

        b.event([](iuic::event_type::pointer_enter e) {
          e.selector.set_hovered(e.uid);
        });

        b.event([](iuic::event_type::pointer_exit e) {
          if (e.selector.is_hovered(e.uid)) {
            e.selector.unset_hovered(e.uid);
          }
        });

        b.event([](iuic::event_type::key_h e) {
          if (e.code == iuic::key_code{(iuic::key_t)1}) {
            if (not e.selector.is_focused(e.uid)) {
              e.selector.set_focused(e.uid);
            }
          }
        });

        b.event([](iuic::event_type::key_f e) {
          std::println("Text field press : {}", e.code[0]);
          if (not e.selector.is_hovered(e.uid) &&
              e.code == iuic::key_code{(iuic::key_t)1}) {
            e.selector.unset_focused(e.uid);
          }
        });
      },
      style);
}

template <auto object> constexpr auto &static_ref() noexcept { return object; }

int main() {
  using namespace iuic;

  std::vector<iuic::key_t> kts{2, 4, 6};

  std::array<iuic::key_t, 2> ar{2, 4};

  constexpr auto ss = sizeof(ar);

  iuic::key_code kcS{ar};

  iuic::key_code kc{kts};

  context ctx;

  ui_size sz{1920, 1080};

  ctx.set_view_size(sz);

  auto ref = ctx.storage.get_ref(2);

  ctx.storage.bind("CZ-2203", &sz);
  ctx.storage.emplace("UUJ", int{22});

  std::cout << "complite" << std::endl;

  ctx.event.pointer({22, 44});

  // SDL BASE
  SDL_Init(SDL_INIT_VIDEO);
  SDL_InitFlags window_flags =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window{SDL_CreateWindow("IUIC - Test", 600, 800, window_flags)};
  SDL_Renderer *renderer{SDL_CreateRenderer(window, nullptr)};
  SDL_SetRenderVSync(renderer, 1);
  // END

  if (not window) {
    std::cout << "ERR : Window not ceated" << std::endl;
  }
  if (not renderer) {
    std::cout << "ERR : Renderer not ceated" << std::endl;
  }
  SDL_Event e;

  bool quit{false};

  while (!quit) {

    ctx.make([](auto &b) {
      // frame(create_info,childs_lambda)
      b.frame([](auto &b) { b.frame([](auto &b) { b.frame(); }, s_1); });
      b.text("test text");

      b.text("ok");
      b.image({});

      button(b, [](iuic::event_type::key_h e) {});
      button(b, [](iuic::event_type::key_h e) {});
      button(b, "touch me");
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
    ctx.set_view_size({static_cast<upixel_t>(h), static_cast<upixel_t>(w)});

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
              iuic::color c = obj.background.color;

              // std::println("r:{}g:{}:b{}:a{}", c.r, c.g, c.b, c.a);

              SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
              SDL_RenderFillRect(renderer, &rect);
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
