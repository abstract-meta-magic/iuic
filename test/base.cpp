#include <raylib.h>

import std;
import iuic.state;
import iuic.core;
import iuic.kitty_kit;
import iuic.keymap.base;

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

template <auto obj> constexpr auto inline_style{obj()};

template <std::invocable<iuic::style::decl> auto obj>
constexpr auto inline_style<obj>{obj(iuic::style::decl{})};

template <std::invocable<iuic::style::shape> auto obj>
constexpr auto inline_style<obj>{obj(iuic::style::shape{})};

void main_window(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;

  // single element
  form::frame<inline_style<[](iuic::style::decl style) {
    style.shape.min_size.height = iuic::units::percent{4};
    style.shape.min_size.width = iuic::units::percent{100};
    style.decoration.background = iuic::units::color{0, 0, 30};
    return style;
  }>>(b, [&](iuic::scheme::builder &b) {
    containers::boxes::lrc(b, [&](auto &b) {
      auto line = [](auto &b) static { form::vline<{255, 255, 255}, 4>(b); };
      auto sline = [](auto &b) static {
        form::vline<{255, 255, 255}, 4, 70.f>(b);
      };
      auto srect = [](auto &b) static {
        static constexpr auto style = []() {
          iuic::style::decl res{};
          res.shape.min_size.width = iuic::units::percent{6};
          res.shape.min_size.height = iuic::units::percent{100};
          res.decoration.background = iuic::units::color{44, 44, 44};

          return res;
        }();
        form::rect<style>(b);
      };

      text::label(b, "Hello", "Fira Code - 24");

      srect(b);

      line(b);

      srect(b);

      line(b);

      srect(b);

      line(b);

      srect(b);

      line(b);

      form::span(b);

      line(b);

      form::rect<inline_style<[](iuic::style::decl style) {
        style.shape.min_size.height = iuic::units::percent{100};
        style.shape.min_size.width = iuic::units::percent{8};
        style.decoration.background = iuic::units::color{44, 44, 44};
        return style;
      }>>(b);

      line(b);
      form::span(b);
      sline(b);
      form::span(b);
      sline(b);
      form::span(b);
    });
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
  struct DC : iuic::text::decoder {
    std::vector<text::glyph::id_t> decode(std::string_view text) override {
      std::vector<text::glyph::id_t> res;
      for (auto ch : text) {
        res.push_back(ch);
      }
      return res;
    };

    const text::decoder::capabilities_t &capabilities() const override {
      static auto __ = capabilities_t{}.set_std_char_support();
      return __;
    };
  };
  auto b = iuic::text::atlas::construct("Fira Code - 24");

  for (int i{0}; i <= 255; ++i) {
    b.link_meta(i, {});
    std::println("set id : {};", i);
  };

  b.set_etalon(4);
  b.set_monospace();
  auto fira_a = b.finalize();
  fira_a.decoder = std::unique_ptr<text::decoder>(new DC{});

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
  SetTargetFPS(144);
  // END

  InitAudioDevice();

  std::vector<Sound> sounds;
  std::size_t sound_id{0};
  for (auto i : index_range{12}) {
    sounds.push_back(LoadSound("../tmp_solutions/res/pop-up.mp3"));
  };
  app app;

  auto mouse_position = GetMousePosition();

  while (not WindowShouldClose() && not app.quit) {

    units::ui::size viewport{(units::upixel)GetScreenWidth(),
                             (units::upixel)GetScreenHeight()};

    ctx.make(viewport, [&](auto &b) { main_window(b, app); }); // iuic test

    units::ui::position pointer{GetMouseX(), GetMouseY()};

    auto in__ = [=](const units::ui::rect area) {
      return pointer.x >= area.x && pointer.x <= area.x + area.w &&
             pointer.y >= area.y && pointer.y <= area.y + area.h;
    };

    // обход по площади
    // обход с zorder
    // discarted | virtualized
    // machine
    // events
    // static | stick | absolute

    for (auto el : ctx.scheme.ranges.postorder()) {
      ctx.scheme.event.list(el);
      auto &area = ctx.scheme.props.area(el);

      if (not ctx.scheme.state.has(el, state::base::hovered) &&
          in__(area.bordered)) {
        if (auto policy = ctx.scheme.props.hovered_policy(el);
            policy == iuic::policy::hovered::propagate) {
          ctx.scheme.state.attach(el, state::base::hovered);
          PlaySound(sounds[sound_id = (sound_id += 1) % 12]);
        } else if (policy == iuic::policy::hovered::block) {
          ctx.scheme.state.attach(el, state::base::hovered);
          PlaySound(sounds[sound_id = (sound_id += 1) % 12]);
          break;
        }
      } else if (ctx.scheme.state.has(el, state::base::hovered)) {
        if (not in__(area.bordered)) {
          ctx.scheme.state.detach(el, state::base::hovered);
        }
      }
    };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      ctx.scheme.global.set_key_code(keymap::mouse("left"));
      for (auto el : ctx.scheme.ranges.level_order()) {
        if (ctx.scheme.state.has(el, state::base::hovered)) {
          for (auto &e : ctx.scheme.event.list(el)) {
            ctx.scheme.event.trigger(e);
          }

          if (ctx.scheme.props.is_discarded(el) ||
              ctx.scheme.props.is_virtualized(el)) {

            // aeu
          }
        }
      }

    } else {
      ctx.scheme.global.set_key_code({});
    }

    BeginDrawing();
    ClearBackground(WHITE);

    for (auto el : ctx.scheme.ranges.level_order()) {
      auto style = ctx.scheme.props.style(el);
      auto &area = ctx.scheme.props.area(el);
      auto &shape = style.get_shape();
      auto &decor = style.get_decoration();

      if (ctx.scheme.props.has_text(el)) {
        auto text = ctx.scheme.props.text(el);
      }

      std::visit(
          [&]<typename type>(const type &obj) {
            if constexpr (std::same_as<type, units::color>) {
              DrawRectangle(area.bordered.x, area.bordered.y, area.bordered.w,
                            area.bordered.h, {obj.r, obj.g, obj.b, obj.a});
            }
          },
          decor.border);

      std::visit(
          [&]<typename type>(const type &obj) {
            if constexpr (std::same_as<type, units::color>) {
              DrawRectangle(area.borderless.x, area.borderless.y,
                            area.borderless.w, area.borderless.h,
                            {obj.r, obj.g, obj.b, obj.a});
            }
          },
          decor.background);
    }

    DrawFPS(0, 0);

    EndDrawing();
  }

  UnloadImage(image);
  for (auto &s : sounds) {
    UnloadSound(s);
  }
  CloseWindow();

#endif

  return 0;
}
