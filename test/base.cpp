#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <print>
#include <string>
#include <string_view>
#include <strings.h>
#include <sys/types.h>
#include <type_traits>
#include <variant>

import iuic.core;
import iuic.kitty_kit;

SDL_FRect to_sdl_rect(const iuic::ui_rect &val) {
  SDL_FRect res;

  res.h = val.size.h;
  res.w = val.size.w;
  res.x = val.position.x;
  res.y = val.position.y;

  return res;
};

using builder_ui = iuic::context::builder;

struct settings_t {};

struct app {
  settings_t settings;

  bool quit{false};
};

void main_window(iuic::context::builder &b, app &app) {
  kitty_kit::button(b, []() { std::println("first"); });
  kitty_kit::button(b, []() { std::println("second"); });
  kitty_kit::button(b, [&]() { app.quit = true; });

  kitty_kit::text_button(b, "ok", []() {});
};

void sdl_event_to_iuic(iuic::context &ctx, app &app) {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_EVENT_QUIT) {
      app.quit = true;
    } else if (e.type == SDL_EVENT_KEY_DOWN) {
      ctx.event.key(iuic::key_code{static_cast<iuic::key_t>(e.key.scancode)});
    } else if (e.type == SDL_EVENT_MOUSE_MOTION) {
      // test pointer event

      ctx.event.pointer_move({(int)e.motion.x, (int)e.motion.y});
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      ctx.event.key(iuic::key_code{e.button.button});
    };
  }
};

void check_resize(iuic::context &ctx, SDL_Window *window) {
  int w, h;
  SDL_GetWindowSizeInPixels(window, &w, &h);
  ctx.set_view_size(
      {static_cast<iuic::upixel_t>(w), static_cast<iuic::upixel_t>(h)});
}

int main() {
  using namespace iuic;

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
    std::println("ERR : Window not ceated");
  }
  if (not renderer) {
    std::println("ERR : Renderer not ceated");
  }

  // IUIC
  context ctx;
  app app;

  while (not app.quit) {

    ctx.make([&](auto &b) { main_window(b, app); }); // iuic test

    sdl_event_to_iuic(ctx, app);

    check_resize(ctx, window);

    //
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // std::cout << "GO" << std::endl;

    ctx.scheme.explore(
        [&](iuic::scheme::frame frame) {
          auto sdl_rect = to_sdl_rect(frame.rect);

          std::visit(
              [&](auto &bg) {
                using type = std::remove_cvref_t<decltype(bg)>;
                if constexpr (std::same_as<type, iuic::color_t>) {
                  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
                  SDL_RenderFillRect(renderer, &sdl_rect);
                }
              },
              frame.style.get_decoration().background);
        },
        [&](iuic::scheme::text text) {
          // text
          auto sdl_rect = to_sdl_rect(text.rect);

          SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
          SDL_RenderDebugText(renderer, text.rect.position.x,
                              text.rect.position.y,
                              std::string{text.text.nodes[0].text}.c_str());
        });

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
