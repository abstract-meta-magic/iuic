#include <raylib.h>

import iuic.state;
import iuic.core;
import iuic.kitty_kit;
import std;

using builder_ui = iuic::scheme::builder;

struct settings_t {};

struct app {
  settings_t settings;

  bool quit{false};

  std::list<int> data;
  int counter{0};
};

void main_window(iuic::scheme::builder &b, app &app) {
  kitty_kit::button(b, [&]() { app.data.push_back(app.counter++); });
  for (auto &d : app.data) {
    kitty_kit::button(
        b, [&]() { std::println("val : {}", d); }, iuic::utils::anchor{d});
  }
  kitty_kit::button(b, [&]() {
    if (not app.data.empty())
      app.data.pop_front();
  });

  kitty_kit::text_button(b, "ok", []() { std::println("ok"); });
  kitty_kit::text_button(b, "exit", [&]() { app.quit = true; });
};

constexpr iuic::style::font::decl base{};

constexpr iuic::style::font::decl regular{base};

constexpr iuic::style::font::decl italic{base, regular};

constexpr iuic::style::font::decl bold{base, italic};

constexpr iuic::style::font::decl bold_second{bold};

constexpr iuic::style::font::decl bold_third{bold_second};

constexpr iuic::external::type opengl{};

constexpr iuic::external::type opengl_text{opengl};

int main() {
  // iuic::test::run();

  using namespace iuic;

  // SDL BASE
  SetWindowState(FLAG_WINDOW_RESIZABLE);
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
    decode(const text::token &t) const noexcept override {
      std::vector<text::glyph::index_t> res{};
      for (auto &c : t.text) {
        res.push_back(c);
      }
      return res;
    };
  };

  struct b : iuic::external::binding {
    // test
    constexpr const external::type &type() const noexcept override {
      return opengl_text;
    };

    constexpr std::string_view info() const noexcept override {
      return "Kitty kit gl binding";
    };
  };
  std::shared_ptr<text::fontset> base_font = text::fontset{}.bind(
      kitty_kit::style::font_base,
      text::glyph::atlas{std::unique_ptr<text::glyph::decoder>{new d{}},
                         std::unique_ptr<external::binding>{new b{}}});

  ctx.font.link(base_font);

  app app;

  auto mouse_position = GetMousePosition();

  while (not WindowShouldClose() && not app.quit) {

    units::ui::size viewport{(units::upixel)GetScreenWidth(),
                             (units::upixel)GetScreenHeight()};

    ctx.make(viewport, [&](auto &b) { main_window(b, app); }); // iuic test

    // TODO :

    static constexpr auto in = [](units::ui::position,
                                  units::ui::rect) -> bool { return true; };

    ctx.scheme.explore([](iuic::scheme::eval::context &ctx) {
      auto el = ctx.self();
      ctx.parent_of(el);
      ctx.childs_of(el);

      ctx.style_of(el);
      ctx.area_of(el);
      ctx.hovered_policy_of(el);
      ctx.event_policy_of(el);

      ctx.select(el);

      if (in(ctx.get_pointer_position(), ctx.area_of(el).bordered)) {
        ctx.attach_state(el, state::base::hovered);
      }
    });

    // std::cout << "GO" << std::endl;
    BeginDrawing();
    ClearBackground(WHITE);

    DrawFPS(0, 0);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
