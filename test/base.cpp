#include <chrono>
#include <cstdint>
#include <expected>
#include <initializer_list>
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <string_view>
#include <strings.h>
#include <sys/types.h>
#include <type_traits>
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

  // auto font_id = ctx.font.atlases.append("ru-stable",atlas);
  // auto& = ctx.font.atlases.get("ru-stable");
  // auto& = ctx.font.atlases.get(font_id);
  // ctx.font.set_proto_builder(...);
  // only text_utils
  // auto& a_1 = utils.atlas_by_id(font_id);
  // auto& a_2 = utils.atlas_by_name("ru-stable");

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
          // ...
        });

    DrawFPS(0, 0);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
