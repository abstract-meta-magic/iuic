module;
#include <raylib.h>

module iuic.backend;
import std;
import iuic.keymap.base;

namespace {
using namespace iuic;

std::vector<iuic::text::atlas> init_fonts() {
  struct DC : iuic::text::decoder {
    std::vector<iuic::text::glyph::id_t>
    decode(std::string_view text) override {
      std::vector<iuic::text::glyph::id_t> res;
      for (auto ch : text) {
        res.push_back(ch);
      }
      return res;
    };

    const iuic::text::decoder::capabilities_t &capabilities() const override {
      static auto __ = capabilities_t{}.set_std_char_support();
      return __;
    };
  };
  std::vector<iuic::text::atlas> res;

  res.emplace_back(
      iuic::text::atlas{iuic::text::atlas::construct("Fira Code - 16")
                            .set_text_height(iuic::units::upixel{16})
                            .set_baseline_offset(iuic::units::upixel{16})
                            .set_monospace()
                            .set_decoder(DC{})
                            .fill_glyph_map([](auto f) {
                              for (int i{0}; i <= 255; ++i) {
                                f.link_advance(i, iuic::units::upixel{8});
                              };
                            })
                            .finalize()});

  res.emplace_back(
      iuic::text::atlas{iuic::text::atlas::construct("Fira Code - 24")
                            .set_text_height(iuic::units::upixel{24})
                            .set_baseline_offset(iuic::units::upixel{24})
                            .set_monospace()
                            .set_decoder(DC{})
                            .fill_glyph_map([](auto f) {
                              for (int i{0}; i <= 255; ++i) {
                                f.link_advance(i, iuic::units::upixel{12});
                              };
                            })
                            .finalize()});

  return res;
};

struct raylib_backend : iuic::backend::instance {

  bool is_shutdown() const override { return WindowShouldClose(); };

  void eval(context &ctx) override {

    units::ui::position pointer{static_cast<units::pixel>(GetMouseX()),
                                static_cast<units::pixel>(GetMouseY())};

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

    BeginDrawing();
    ClearBackground(WHITE);
    float scale{1.f};
    if (GetScreenHeight() < GetScreenWidth()) {
      scale = (float)GetScreenWidth() / (float)bg.width;
    } else {
      scale = (float)GetScreenHeight() / (float)bg.height;
    }
    DrawTextureEx(bg_t, {0, 0}, 0.f, scale, {255, 255, 255, 255});

    auto to_raylib_rect = [](const iuic::units::ui::rect &rect) {
      struct ray_rect {
        int h, w, x, y;
      };
      return ray_rect{
          .h = static_cast<int>(rect.h),
          .w = static_cast<int>(rect.w),
          .x = static_cast<int>(rect.x),
          .y = static_cast<int>(rect.y),
      };
    };

    for (auto el : ctx.scheme.ranges.level_order()) {
      auto style = ctx.scheme.props.style(el);
      auto &area = ctx.scheme.props.area(el);
      auto &shape = style.get_shape();
      auto &decor = style.get_decoration();

      if (ctx.scheme.props.has_text(el)) { // draw text
        auto text = ctx.scheme.props.text(el);

        auto area = ctx.scheme.props.area(el);
        auto brect = to_raylib_rect(area.borderless);
        // DrawRectangle(brect.x, brect.y, brect.w, brect.y, {0, 255, 255,
        // 100});

        for (auto &token : text) {
          //
          auto &atlas = iuic::text::atlas::by_id(token.atlas_id);
          auto y_ = token.baseline.y - atlas.baseline_offset;
          auto h_ = atlas.text_height;
          auto w_ = atlas.glyphs.advance[0];

          for (auto &glyph : token.placement) {
            DrawRectangle(static_cast<std::int32_t>(token.baseline.x) +
                              static_cast<std::int32_t>(glyph.offset),
                          static_cast<std::int32_t>(y_),
                          static_cast<std::uint32_t>(w_),
                          static_cast<std::uint32_t>(h_), {255, 0, 0, 255});
          };
        };

      } else { // draw frame
        std::visit(
            [&]<typename type>(const type &obj) {
              if constexpr (std::same_as<type, units::color>) {
                auto rect = to_raylib_rect(area.bordered);
                DrawRectangle(rect.x, rect.y, rect.w, rect.h,
                              {obj.r, obj.g, obj.b, obj.a});
              }
            },
            decor.border);

        std::visit(
            [&]<typename type>(const type &obj) {
              if constexpr (std::same_as<type, units::color>) {
                auto rect = to_raylib_rect(area.borderless);
                DrawRectangle(rect.x, rect.y, rect.w, rect.h,
                              {obj.r, obj.g, obj.b, obj.a});
              }
            },
            decor.background);
      }
    }

    DrawFPS(0, 0);

    EndDrawing();
  };

  units::ui::size get_viewport_size() override {
    return units::ui::size{static_cast<units::upixel>(GetScreenWidth()),
                           static_cast<units::upixel>(GetScreenHeight())};
  };

  void set_viewport_size(units::ui::size) override{};

  raylib_backend() {
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(400, 300);
    SetTargetFPS(144);
    InitWindow(800, 600, "RayBack");
    bg = LoadImage("./source/bg.png");
    bg_t = LoadTextureFromImage(bg);
  };
  ~raylib_backend() {
    UnloadTexture(bg_t);
    UnloadImage(bg);
    CloseWindow();
  };

  std::vector<text::atlas> fonts{init_fonts()};
  Image bg;
  Texture bg_t;
};
}; // namespace

namespace iuic::backend {
instance &instance::get() {
  static raylib_backend _{};

  return _;
};
}; // namespace iuic::backend
