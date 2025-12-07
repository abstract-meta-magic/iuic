#include <chrono>
#include <cstdint>
#include <expected>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <string_view>
#include <strings.h>
#include <sys/types.h>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

#include <raylib.h>

import iuic.core;
import iuic.kitty_kit;

using builder_ui = iuic::context::builder;

struct settings_t {};

struct app {
  settings_t settings;

  bool quit{false};
};

void main_window(iuic::context::builder &b, app &app) {
  kitty_kit::button(b, []() { std::println("my"); });
  kitty_kit::button(b, []() { std::println("my"); });
  kitty_kit::button(b, []() { std::println("my"); });
  kitty_kit::text_button(b, "ok", []() { std::println("ok"); });
  kitty_kit::text_button(b, "exit", [&]() { app.quit = true; });
};

constexpr iuic::style::font::decl base{};

constexpr iuic::style::font::decl regular{base};

constexpr iuic::style::font::decl italic{base, regular};

constexpr iuic::style::font::decl bold{base, italic};

constexpr iuic::style::font::decl bold_second{bold};

constexpr iuic::style::font::decl bold_third{bold_second};

constexpr iuic::extern_type opengl{};

constexpr iuic::extern_type opengl_text{opengl};

int main() {
  using namespace iuic;

  // SDL BASE
  SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED);
  InitWindow(600, 800, "iuic-test");
  SetWindowMinSize(400, 300);
  SetTargetFPS(140);
  // END

  // IUIC
  context ctx;
  app app;

  // fontset my{};
  // сосотоит из атласов и линкуеться к ctx
  // ctx.font.link(my);            // std::shared_ptr ???
  // ctx.font.get(by font::cref);  // const atlas &

  // auto f (= ctx.font.get(base);
  // f.discriptor;
  // vertex \ frag \ texture \ [ubo\ssbo]

  // dset

  ctx.set_view_size({600, 800});

  auto mouse_position = GetMousePosition();

  std::println("index 0 {}", bold.get_index());
  std::println("index 1 {}", bold.get_underlying()->get_index());
  std::println("index 1 {}", italic.get_underlying()->get_index());

  while (not WindowShouldClose() && not app.quit) {

    ctx.set_view_size(
        {(upixel_t)GetScreenWidth(), (upixel_t)GetScreenHeight()});

    ctx.make([&](auto &b) { main_window(b, app); }); // iuic test

    auto new_mouse_position = GetMousePosition();
    if (mouse_position.x != new_mouse_position.x ||
        mouse_position.y != new_mouse_position.y) {
      std::exchange(mouse_position, new_mouse_position);
      ctx.event.pointer_move({(int)mouse_position.x, (int)mouse_position.y});
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      ctx.event.key(iuic::key_map::mouse("left"));
    } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
      ctx.event.key(iuic::key_map::mouse("right"));
    }

    // std::cout << "GO" << std::endl;
    BeginDrawing();
    ClearBackground(WHITE);
    // сначала эксперимент на gl

    //

    std::println("---------------------- BEGIN");
    ctx.scheme.explore(
        [](iuic::scheme::frame frame) {
          std::visit(
              [&](auto &obj) {
                using type = std::remove_cvref_t<decltype(obj)>;
                if constexpr (std::same_as<type, iuic::color_t>) {
                  auto [x, y, w, h] = frame.rect.xywh();
                  DrawRectangle(x, y, w, h, Color{obj.r, obj.g, obj.b, obj.a});
                }
              },
              frame.style.get_decoration().background);
        },
        [](iuic::scheme::text text) {
          // ctx.font.get(text.style.get_advance().text.font);

          std::println("---------------------- TTB");
          for (auto &&glyph : text.text) {
            std::println("glyph info : id-{} , x-{} , y-{}", glyph.id,
                         glyph.position.x, glyph.position.y);
          }
          std::println("---------------------- TTE");
          // ...
        });
    std::println("---------------------- END");

    DrawFPS(0, 0);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
