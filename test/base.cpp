#include <chrono>
#include <cstdint>
#include <expected>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
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
#include <vector>

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

  // ctx font init
  struct d : text::glyph::decoder {
    // test
    constexpr std::expected<std::vector<text::glyph::index_t>, error>
    decode(const text::token &) const noexcept override {
      return std::vector<text::glyph::index_t>{0};
    };
  };

  struct b : iuic::extern_binding {
    // test
    constexpr const extern_type &type() const noexcept override {
      return opengl_text;
    };

    constexpr std::string_view info() const noexcept override {
      return "Kitty kit gl binding";
    };
  };
  std::shared_ptr<text::fontset> base_font = text::fontset{}.bind(
      kitty_kit::style::font_base,
      text::glyph::atlas{std::unique_ptr<text::glyph::decoder>{new d{}},
                         std::unique_ptr<extern_binding>{new b{}}});

  ctx.font.link(base_font);

  app app;

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

    // code

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

          auto fref = frame.style.get_advance().text.font;
        },
        [&](iuic::scheme::text text) {
          auto fref = text.style.get_advance().text.font;

          if (fref == kitty_kit::style::font_base) {
            std::println("kitty font");
          }

          auto &atlas = ctx.font.get(fref);

          auto &b = atlas.get_binding();

          if (b.type() == opengl_text) {
            // do render

            std::println("BINFO : {}", b.info());
          }
        });

    DrawFPS(0, 0);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
