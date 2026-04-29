import std;
import iuic.state;
import iuic.core;
import iuic.backend;
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

inline void top_panel(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;
  using namespace iuic::units::literals;
  form::frame<inline_style<[](iuic::style::decl style) {
    style.shape.min_size.height = 4_pr;
    style.shape.min_size.width = 100_pr;
    style.decoration.background = iuic::units::color{0, 0, 30};
    return style;
  }>>(b, [&](iuic::scheme::builder &b) {
    containers::boxes::lrc(b, [&](auto &b) {
      auto line = [](auto &b) static { form::vline<"white"_rgb8, 4_upx>(b); };
      auto sline = [](auto &b) static {
        form::vline<"white"_rgb8, 4_upx, 70_pr>(b);
      };

      auto srect = [](auto &b) static {
        static constexpr auto style = []() {
          iuic::style::decl res{};
          res.shape.min_size.width = 6_pr;
          res.shape.min_size.height = 100_pr;
          res.decoration.background = iuic::units::color{44, 44, 44};

          return res;
        }();
        form::rect<style>(b);
      };

      text::label(b, "Hello", "Fira Code - 16");
      line(b);
      form::span(b);
      sline(b);
      form::span(b);
      buttons::box(b, [&]() { app.quit = true; });
      form::span(b);
      sline(b);
      form::span(b);
      line(b);
      text::label(b, "Hello", "Fira Code - 16");
    });
  });
};

inline void mid_panel_stats(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;
  using namespace iuic::units::literals;

  form::frame<inline_style<[](iuic::style::decl style) static {
    style.shape.min_size.width = 420_upx;
    style.shape.min_size.height = 160_upx;
    style.shape.margin.top = 40_upx;
    style.shape.margin.left = 20_upx;
    style.decoration.background = "gray"_rgb8;
    return style;
  }>>(b, [](auto &b) {
    containers::boxes::tbc(b, [](auto &b) {
      static constexpr auto style = []() {
        iuic::style::decl style{};
        style.shape.min_size.width = 80_pr;
        style.shape.min_size.height = 40_upx;
        style.decoration.background = "black"_rgb8;
        return style;
      }();
      form::frame<style>(b, [](auto &b) {});
      form::span(b);
      form::hline<"white"_rgb8, 4_upx, 92_pr>(b);
      form::span(b);

      static constexpr auto style_2 = []() {
        iuic::style::decl style{};
        style.shape.min_size.width = 80_pr;
        style.shape.min_size.height = 64_pr;
        style.decoration.background = "black"_rgb8;
        return style;
      }();

      form::span(b);
      form::frame<style_2>(b, [](auto &b) {
        containers::boxes::lrc(b, [](auto &b) {
          static constexpr auto style_3 = []() {
            iuic::style::decl style{};
            style.shape.min_size.width = 46_pr;
            style.shape.min_size.height = 100_pr;
            style.decoration.background = "blue"_rgb8;
            return style;
          }();
          form::span(b);
          form::frame<style_3>(b, [](auto &b) {
            containers::boxes::tbc(b, [](auto &b) {
              form::span<600_w>(b);

              // TODO : replace to rlc
              containers::boxes::lrc(b, [](auto &b) {
                text::label(b, "day", "Fira Code - 24"); // dynamic in prod
                form::span(b);
                text::label(b, "16", "Fira Code - 24"); // dynamic in prod
              });
              form::span<800_w>(b);
              form::hline<"white"_rgb8>(b);
              form::span<800_w>(b);
              containers::boxes::lrc(b, [](auto &b) {
                text::label(b, "day", "Fira Code - 24"); // dynamic in prod
                form::span(b);
                text::label(b, "16", "Fira Code - 24"); // dynamic in prod
              });
              form::span<600_w>(b);
            });
          });
          form::span(b);
          form::vline<"white"_rgb8, 4_upx, 92_pr>(b);
          form::span(b);
          form::frame<style_3>(b, [](auto &b) {
            containers::boxes::tbc(b, [](auto &b) {
              form::span<600_w>(b);
              containers::boxes::lrc(b, [](auto &b) {
                text::label(b, "day", "Fira Code - 24"); // dynamic in prod
                form::span(b);
                text::label(b, "16", "Fira Code - 24"); // dynamic in prod
              });
              form::span<800_w>(b);
              form::hline<"white"_rgb8>(b);
              form::span<800_w>(b);
              containers::boxes::lrc(b, [](auto &b) {
                text::label(b, "day", "Fira Code - 24"); // dynamic in prod
                form::span(b);
                text::label(b, "16", "Fira Code - 24"); // dynamic in prod
              });
              form::span<600_w>(b);
            });
          });
          form::span(b);
        });
      });
    });
  });
};

inline void mid_panel_characteristics(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;
  using namespace iuic::units::literals;
  form::frame<inline_style<[](iuic::style::decl style) static {
    style.shape.min_size.width = 420_upx;
    style.shape.min_size.height = 160_upx;
    style.shape.margin.top = 40_upx;
    style.shape.margin.left = 20_upx;
    style.decoration.background = "gray"_rgb8;
    return style;
  }>>(b, [](auto &b) {
    containers::boxes::tbc(b, [](auto &b) {
      static constexpr auto style = []() {
        iuic::style::decl res{};
        res.shape.min_size.width = 80_pr;
        res.shape.min_size.height = 100_pr;
        res.decoration.background = "black"_rgb8;

        return res;
      }();
      form::span(b);
      form::frame<style>(b, [](auto &b) {});
      form::span(b);
    });
  });
};

inline void mid_panel(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;
  using namespace iuic::units::literals;

  form::frame<inline_style<[](iuic::style::decl style) {
    style.shape.min_size.width = 100_pr;
    style.shape.min_size.height = 24_pr;
    return style;
  }>>(b, [&](auto &b) {
    containers::boxes::lrc(b, [&](auto &b) {
      form::span<80_w>(b);
      mid_panel_stats(b, app);
      form::span<200_w>(b);
      mid_panel_characteristics(b, app);
      form::span<1800_w>(b);
    });
  });
}

inline void main_window(iuic::scheme::builder &b, app &app) {
  using namespace kitty_kit;
  using namespace iuic::units::literals;
  top_panel(b, app);
  mid_panel(b, app);
};

int main() {
  // iuic::test::run();

  auto &backend = iuic::backend::instance::get();

  using namespace iuic;

  context ctx;

  app app;

  while (not app.quit && not backend.is_shutdown()) {
    ctx.make(backend.get_viewport_size(),
             [&](auto &b) { main_window(b, app); }); // iuic test

    backend.eval(ctx);
  }

  return 0;
}
