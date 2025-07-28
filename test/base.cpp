#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <print>
#include <string_view>
#include <type_traits>
#include <variant>

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

template <iuic::style style = iuic::def_style> void button(auto &b) {
  b.template frame<style>([](auto &b) {
    // events
  });
}
template <iuic::style style = iuic::def_style>
constexpr inline void button(auto &b, std::string_view str) {
  b.template frame<style>([=](auto &b) {
    b.text(str);
    if (auto ref = b.storage.get_ref(int{2}); ref.template as<int>()) {
      auto &obj = ref.template unwrap<int>();
    };

    b.template event<iuic::KeyAction::Down>(
        iuic::keymap::en::qwerty("A", "B"), iuic::KeyMod::None,
        [](auto ref) {

        },
        b.storage.get_key(233));
    b.template event<iuic::KeyAction::Up>(iuic::keymap::en::qwerty("W"),
                                          iuic::KeyMod::Shift, []() {
                                            // ...
                                          });

    b.template event<iuic::KeyAction::Down>(iuic::keymap::en::qwerty("X"),
                                            [](auto &r) {});

    b.template event<iuic::PointerAction::Move>([](iuic::ui_position pos) {
      // ...
    });
    b.template event<iuic::PointerAction::In>([]() {
      // ...
    });

    // b.transform.position.shift_left(20);
    // b.transform.shape.scale(1.2);
  });
}

int main() {
  using namespace iuic;

  context ctx;

  ui_size sz{1920, 1080};

  ctx.set_view_size(sz);

  auto ref = ctx.storage.get_ref(2);

  ctx.storage.bind("CZ-2203", &sz);
  ctx.storage.emplace("UUJ", int{22});

  std::cout << "complite" << std::endl;

  ctx.event.key("A", KeyAction::Down);
  ctx.event.key("B", KeyAction::Down);
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
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    int w, h;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    ctx.set_view_size({static_cast<upixel_t>(h), static_cast<upixel_t>(w)});

    ctx.make([](auto &b) {
      // frame(create_info,childs_lambda)
      b.frame([](auto &b) {
        b.template frame<s_1>([](auto &b) { b.frame([](auto &) {}); });
      });

      b.text("test text");
      b.template text<style{.shape{.min_size{240, 400}},
                            .background{.color{94, 104, 196, 255}}}>("ok");
      b.image({});

      button(b);
      button(b);
      button(b, "touch me");
    });

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    std::cout << "GO" << std::endl;
    auto &tree = ctx.get_tree();
    size_t begin{1}, end{tree.size() - 1};
    for (; begin != end; ++begin) {
      auto &r = tree[begin];
      auto rect = to_sdl_rect(r.area);
      std::visit(
          [renderer, &rect](auto &obj) {
            if constexpr (std::same_as<std::remove_cvref_t<decltype(obj)>,
                                       frame_render_data>) {
              iuic::color c = obj.background.color;

              std::println("r:{}g:{}:b{}:a{}", c.r, c.g, c.b, c.a);

              SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
              SDL_RenderFillRect(renderer, &rect);
            }
          },
          r.data);
    }

    std::cout << "GO" << std::endl;

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
