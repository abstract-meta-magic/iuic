#include <raylib.h>

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
  using namespace iuic::state;

  constexpr auto &a = decl::unique_instance();
  constexpr auto &b = decl::unique_instance();
  constexpr auto &e = decl::unique_instance();

  constexpr auto spec = machine::spec<
      machine::transition_graph<machine::transition{a, b, true}>{}, app>{};

  constexpr auto proto =
      spec.get_protobuilder()
          .entry<b>()
          .transition(a, b,
                      [](const machine::execute::state &state,
                         app &app) -> machine::execute::transition {
                        int i{0};
                        for (; not state.is_interrupted() && ++i <= 4;) {
                          // do job
                          std::println("job a -> b");

                          co_yield machine::execute::result::process;
                        }

                        if (state.is_interrupted()) {
                          co_yield machine::execute::result::success_interrupt;
                        }
                        co_return;
                      })
          .transition(b, a,
                      [](const machine::execute::state &state,
                         app &) -> machine::execute::transition {
                        int i{0};
                        for (; not state.is_interrupted() && ++i <= 2;) {
                          std::println("job b -> a");
                          // do job
                          co_yield machine::execute::result::process;
                        }

                        if (state.is_interrupted()) {
                          co_yield machine::execute::result::success_interrupt;
                        }
                        co_return;
                      })
          .exception_handler([](const machine::execute::state &state,
                                app &) -> machine::execute::stay {
            if (state.exception) {
              std::println("yo");
              co_yield machine::execute::result::
                  terminate_by_unexpected_exception;
            }
            co_return base::idle;
          })
          .stay(b,
                [](const machine::execute::state &state,
                   app &a) -> machine::execute::stay {
                  int i{0};
                  for (; not state.is_interrupted();) {
                    std::println("B num is :{}", ++i);
                    co_yield machine::execute::result::process;
                  };
                  if (state.is_interrupted()) {
                    std::println("interrupt");
                    co_yield machine::execute::result::success_interrupt;
                  }
                  co_return base::idle;
                })
          .stay(a,
                [](const machine::execute::state &state,
                   app &a) -> machine::execute::stay {
                  int i{0};
                  for (; not state.is_interrupted();) {
                    std::println("A num is :{}", ++i);
                    co_yield machine::execute::result::process;
                  };
                  if (state.is_interrupted()) {
                    std::println("interrupt");
                    co_yield machine::execute::result::success_interrupt;
                  }
                  co_return base::idle;
                })
          .terminate([](const machine::execute::state &state,
                        app &) -> machine::execute::transition { co_return; })
          .finalize();

  auto machine = spec.make_instance(proto);

  machine->process();
  machine->process();
  machine->process();
  machine->get_controller().try_move(a);
  machine->process(); // init tr
  machine->process();
  machine->process();
  machine->process();
  machine->process();
  machine->process();
  machine->get_controller().try_move(b);
  machine->process(); // init tr
  machine->process();
  machine->process();
  machine->process();
  machine->get_controller().try_move(a);
  machine->process(); // init tr
  machine->process();
  machine->process();
  machine->process();
  machine->process();
  machine->process();

  /*
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

  ctx.set_view_size({600, 800});

  auto mouse_position = GetMousePosition();

  while (not WindowShouldClose() && not app.quit) {

    ctx.set_view_size(
        {(units::upixel)GetScreenWidth(), (units::upixel)GetScreenHeight()});

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
                if constexpr (std::same_as<type, iuic::units::color_t>) {
                  auto [x, y, w, h] = frame.rect.xywh();
                  Rectangle rect{(float)x, (float)y, (float)w, (float)h};
                  Color color{obj.r, obj.g, obj.b, obj.a};
                  DrawRectangleRec(rect, color);
                  // DrawRectangleRounded(rect, .80f, 20, color);
                }
              },
              frame.style.get_decoration().background);

          auto fref = frame.style.get_advance().text.font;
        },
        [&](iuic::scheme::text text) {
          auto fref = text.style.get_advance().text.font;

          auto &atlas = ctx.font.get(fref);

          auto &b = atlas.get_binding();

          Color color{.r = 0, .g = 0, .b = 0, .a = 255};
          //         auto [x, y, w, h] = text.rect.xywh();
          // DrawRectangle(x - 4, y, w + 8, h, color);

          if (b.type() == opengl_text) {
            for (auto &&g : text.text) {
              DrawRectangle(g.position.x, g.position.y, 6, 12, color);
              color.r += 40;
              color.g += 40;
            }
          }
        });

    DrawFPS(0, 0);

    EndDrawing();
  }

  CloseWindow();
  return 0;
  */
}
