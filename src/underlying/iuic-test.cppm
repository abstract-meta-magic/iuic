// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying:test;
import std;
import :utils;
namespace iuic::test {

export struct utils {
  void eq() {};

  void neq() {};
};

using body_ptr = void (*)(utils &);

struct unit_impl {
  body_ptr body;
  std::string_view name;
  std::string_view group;
  std::string_view desc;
};

export struct ticket {};

export using testset = std::span<const unit_impl>;

ticket add_unit_to_main(unit_impl unit);
struct main {
  static inline ticket add__(unit_impl test_case) {
    instance__().tests.push_back(test_case);
    return {};
  };

  static void run__(std::invocable<testset> auto &&stratagy) {
    instance__().run_impl__(std::forward<decltype(stratagy)>(stratagy));
  };

private:
  void run_impl__(std::invocable<testset> auto &&stratagy) { stratagy(tests); };

  static inline main &instance__() {
    static main _{};
    return _;
  };
  main() = default;
  std::vector<unit_impl> tests{};
};
}; // namespace iuic::test

export namespace iuic::test {

template <body_ptr body, iuic::utils::ct::str name = {"unnamed"},
          iuic::utils::ct::str group = {"global"},
          iuic::utils::ct::str desc = {"no doc"}>
struct unit {
  static inline unit_impl to_impl() {
    return {body, name.to_view(), group.to_view(), desc.to_view()};
  };
};

inline void run(std::invocable<testset> auto &&stratagy) {
  main::run__(std::forward<decltype(stratagy)>(stratagy));
};

inline void run() {
  constexpr auto st = [](testset testset) {
    std::println("IUIC-TEST-RUN[BEGIN]|> ------------");
    std::println();
    for (auto &&test : testset) {
      std::println("IUIC-TEST|---------------");
      std::println("NAME  : {}", test.name);
      std::println("GROUP : {}", test.group);
      std::println("DESC  : {}", test.desc);
      std::println("----------START----------");
      if (test.body) {
        utils u;
        try {
          test.body(u);
        } catch (...) {
          // do
        }
      }
      std::println("-----------END-----------");
      std::println("---------------|IUIC-TEST");
      std::println();
    }
    std::println("IUIC-TEST-RUN[END]|| -------------");
  };

  run(st);
};

template <unit case__> inline ticket usecase{main::add__(case__.to_impl())};
}; // namespace iuic::test
