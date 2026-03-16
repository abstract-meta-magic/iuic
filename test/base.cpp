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

struct index_range {
  struct iterator_base {

    iterator_base(std::size_t index_) : index{index_} {}

    friend bool operator==(const index_range::iterator_base &lhs,
                           const index_range::iterator_base &rhs);

  protected:
    std::size_t index{0};
  };
  struct sentinel : iterator_base {
    sentinel(std::size_t index) : iterator_base{index} {}
  };

  struct iterator : iterator_base {

    iterator &operator++() { return ++index, *this; }

    std::size_t operator*() { return index; }

    iterator(std::size_t index) : iterator_base{index} {}
  };

  iterator begin() { return {begin_}; };

  sentinel end() { return {end_}; };

  index_range(std::size_t end) : begin_{0}, end_{end} {}
  index_range(std::size_t begin, std::size_t end) : begin_{begin}, end_{end} {}

private:
  std::size_t begin_{0};
  std::size_t end_{0};
};

bool operator==(const index_range::iterator_base &lhs,
                const index_range::iterator_base &rhs) {
  return lhs.index >= rhs.index;
}

void main_window(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;

  containers::boxes::left_top(b, [&](auto &b) {
    for (auto i : index_range{3}) {
      buttons::box(b, [&]() { std::println("hah"); });
    }
    buttons::box(b, [&]() { app.quit = true; });
  });
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

  // IUIC

  context ctx;
#if 0
  ctx.make({400, 800}, [](auto &b) {
    kitty_kit::button(b, []() { std::println("triggered");});
    kitty_kit::button(b, []() {});
    kitty_kit::button(b, []() {});
  });

  std::vector<units::uid> sl;

  ctx.scheme.explore(
      [&](scheme::eval::context &ctx) {
        // hovered
        auto area = ctx.self_area();
      },
      [&](scheme::geval::context &ctx) {
        for (auto uid : sl) {
          ctx.attach_state(uid, state::base::hovered);
        }
      },
      [&](scheme::eval::context &ctx) {
        // for events
      });
#endif

#if 1

  Image image = LoadImage("test/res/image.png");

  std::println("failure image load");
  // RAYLIB BASE
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  InitWindow(600, 800, "iuic-test");
  SetWindowMinSize(400, 300);
  SetTargetFPS(140);
  // END

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

  // ctx.font.link(base_font);

  app app;

  auto mouse_position = GetMousePosition();

  while (not WindowShouldClose() && not app.quit) {

    units::ui::size viewport{(units::upixel)GetScreenWidth(),
                             (units::upixel)GetScreenHeight()};

    ctx.make(viewport, [&](auto &b) { main_window(b, app); }); // iuic test

    // TODO :

    static constexpr auto in = [](units::ui::position,
                                  units::ui::rect) -> bool { return true; };

    std::set<units::uid> sel;
    std::set<units::uid> ev;

    ctx.scheme.explore(
        [&](scheme::eval::context &ctx) {
          auto inner = [](const units::ui::area &area,
                          units::ui::position pointer) -> bool {
            return pointer.x >= area.bordered.x &&
                   pointer.x <= area.bordered.x + area.bordered.w &&
                   pointer.y >= area.bordered.y &&
                   pointer.y <= area.bordered.y + area.bordered.h;
          };

          auto area = ctx.self_area();

          if (inner(area, {GetMouseX(), GetMouseY()})) {
            std::size_t uid = ctx.self_uid();
            sel.insert(uid);
          }

          if (ctx.has_state(state::base::hovered)) {
            ev.insert(ctx.self_uid());
          };
        },
        [&](iuic::scheme::geval::context &ctx) {
          if (IsMouseButtonPressed(0)) {
            ctx.set_key(key_map::mouse("left"));
            std::println("-------------------");
          } else {
            ctx.set_key({});
          }

          for (auto uid : sel) {
            ctx.attach_state(uid, state::base::hovered);
          }

          for (auto uid : ev) {

            for (auto e : ctx.events_of(uid)) {
              ctx.event_trigger(e);
            }
          }
        },
        [&](iuic::scheme::eval::context &ctx) {
          auto area = ctx.self_area();

          auto style = ctx.self_style();

          std::visit(
              [&]<typename type>(const type &obj) {
                if constexpr (std::same_as<type, units::color>) {
                  Color color{obj.r, obj.g, obj.b, obj.a};
                  DrawRectangle(area.bordered.x, area.bordered.y,
                                area.bordered.w, area.bordered.h, color);
                }
              },
              style.get_decoration().border);

          std::visit(
              [&]<typename type>(const type &obj) {
                if constexpr (std::same_as<type, units::color>) {
                  Color color{obj.r, obj.g, obj.b, obj.a};
                  DrawRectangle(area.borderless.x, area.borderless.y,
                                area.borderless.w, area.borderless.h, color);
                  Rectangle rect{.x = (float)area.borderless.x,
                                 .y = (float)area.borderless.y,
                                 .width = (float)area.borderless.w,
                                 .height = (float)area.borderless.h};
                }
              },
              style.get_decoration().background);
        });

    // std::cout << "GO" << std::endl;
    BeginDrawing();
    ClearBackground(WHITE);

    DrawFPS(0, 0);

    EndDrawing();
  }

  UnloadImage(image);
  CloseWindow();

#endif

  return 0;
}
