#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <print>
#include <string_view>
#include <strings.h>
#include <type_traits>
#include <variant>
#include <vector>

import iuic.core;

namespace iuic::key_map {
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

  res.positioning.margin.top = 20;
  res.positioning.margin.left = 30;

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

  b.frame(
      [&](auto &b) {
        // just for uid
        b.frame([&](auto &b) { uid = b.make_uid(&uid_seed); },
                none_style); // hiden style

        auto srk = b.storage.persist(uid, "callback");

        b.storage.init_if_not(srk, [&]() { return call; });

        b.apply_uid(uid); // event set

        b.event([](iuic::event_type::pointer_enter e) {
          e.selector.set_hovered(e.uid);
        });

        b.event([](iuic::event_type::pointer_exit e) {
          if (e.selector.is_hovered(e.uid)) {
            e.selector.unset_hovered(e.uid);
          }
        });

        using callback_t = std::remove_cvref_t<decltype(call)>;
        b.event(
            [](iuic::event_type::key_h e) {
              if (e.code == iuic::key_map::mouse("left")) {
                e.storage.try_visit(e.srk, [](callback_t &call) { call(); });
              };
            },
            srk);
      },
      style);
}

constexpr inline void button(iuic::context::builder &b, const std::string &str,
                             const iuic::style &style = iuic::def_style) {
  static constexpr bool uid_sub_seed{true};

  auto uid = b.make_uid(b.make_uid(&uid_sub_seed), str);

  b.frame(
      [=](auto &b) {
        // b.text(str);

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
          if (e.code == iuic::key_map::mouse("left")) {
            if (not e.selector.is_focused(e.uid)) {
              e.selector.set_focused(e.uid);
            }
          }
        });

        b.event([](iuic::event_type::key_f e) {
          std::println("Text field press : {}", e.code[0]);
          if (not e.selector.is_hovered(e.uid) &&
              e.code == iuic::key_map::mouse("left")) {
            e.selector.unset_focused(e.uid);
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

  static constexpr srk_t text_field() noexcept;

  static constexpr srk_t text_field(std::string) noexcept;

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
        res.positioning.margin = {20, 20, 20, 20};

        return res;
      }>();
      // frame(create_info,childs_lambda)
      b.frame([](auto &b) { b.frame([](auto &b) { b.frame(); }, style); });
      // b.text("test text");

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
