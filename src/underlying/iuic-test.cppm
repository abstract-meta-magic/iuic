// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module;

#include <backtrace.h>
#include <cxxabi.h>

export module iuic.underlying.test;
import std;
import iuic.underlying.utils;

namespace iuic::test {

void trace_if_supported() {};

struct assert_base {
  constexpr operator bool() const noexcept { return value; }

  constexpr assert_base &assertion_true(std::string_view msg) & {
    if_true = msg;
    return *this;
  };

  constexpr assert_base &assertion_false(std::string_view msg) & {
    if_false = msg;
    return *this;
  };

  constexpr assert_base(bool v) : value{v} {}

protected:
  std::string_view if_true{""};
  std::string_view if_false{""};
  bool value{true};
};

struct assert : assert_base {
  constexpr assert(bool value, std::string diagnostic_)
      : assert_base{value}, diagnostic{diagnostic_} {}

  constexpr assert(bool value) : assert_base{value} {}

  constexpr std::string_view assert_msg() const noexcept {
    return value ? if_true : if_false;
  };

  constexpr std::string_view diagnostic_msg() const noexcept {
    return diagnostic;
  };

  constexpr bool has_assert_msg() const noexcept {
    return not assert_msg().empty();
  };

  constexpr bool has_diagnostic() const noexcept {
    return not diagnostic.empty();
  };

private:
  std::string diagnostic{""};
};

struct benchmark {
  using time_point =
      std::chrono::time_point<std::chrono::high_resolution_clock>;
  using value_t = std::chrono::nanoseconds;

  benchmark &limit(value_t l) & {
    limit_ = l;
    return *this;
  };

  benchmark &name(std::string_view str) & {
    name_ = str;
    return *this;
  };

  benchmark(value_t res) : middle_{res}, lower_{res}, upper_{res} {}

  benchmark(value_t l, value_t m, value_t u)
      : middle_{m}, lower_{l}, upper_{u} {}

  value_t get_lower() const { return lower_; };
  value_t get_upper() const { return upper_; };
  value_t get_middle() const { return middle_; };

  std::string_view get_name() const { return name_; };

private:
  std::string name_{"unnamed"};
  value_t middle_{0};
  value_t lower_{0};
  value_t upper_{0};
  value_t limit_{0};

  // lower
  // middle
  // upper
};

inline void backtrace_err(void *d, const char *msg, int err) {}

inline int backtrace_full(void *data, uintptr_t pc, const char *filename,
                          int lineno, const char *function) {
  if (function) {
    int status{-1};
    std::unique_ptr<char, void (*)(void *)> res(
        abi::__cxa_demangle(function, nullptr, nullptr, &status), std::free);

    std::string_view fn{res.get()};

    if (fn.contains("iuic::test::utils@iuic.underlying&")) {
      std::println("FILE: {}:{}:0", filename, lineno);
      return -1;
    };
  }
  return 0;
};

export struct utils {
  template <typename T, typename U> assert_base &eq(const T &exp, U &&dir) {
    if (exp != dir) {
      std::println(">>>>>>>>>>>");
      std::println("EQ  - FALSE");
      trace_if_supported();
      auto *st = backtrace_create_state(nullptr, 0, backtrace_err, nullptr);
      backtrace_full(st, 0, backtrace_full, backtrace_err, nullptr);
      std::println("<<<<<<<<<<<");
      asserts.push_back(false);
    } else {
      asserts.push_back(true);
    }
    return asserts.back();
  };

  template <typename T> assert_base &eq(const T &exp) {
    if (not exp) {
      std::println(">>>>>>>>>>>");
      std::println("EQ  - FALSE");
      trace_if_supported();
      auto *st = backtrace_create_state(nullptr, 0, backtrace_err, nullptr);
      backtrace_full(st, 0, backtrace_full, backtrace_err, nullptr);
      std::println("<<<<<<<<<<<");
      asserts.push_back(false);
    } else {
      asserts.push_back(true);
    }
    return asserts.back();
  };

  template <typename T, typename U> assert_base &neq(const T &exp, U &&dir) {
    if (exp == dir) {
      std::println(">>>>>>>>>>>");
      std::println("NEQ - FALSE");
      auto *st = backtrace_create_state(nullptr, 0, backtrace_err, nullptr);
      backtrace_full(st, 0, backtrace_full, backtrace_err, nullptr);
      std::println("<<<<<<<<<<<");
      asserts.push_back(false);
    } else {
      asserts.push_back(true);
    }
    return asserts.back();
  };

  template <typename T> assert_base &neq(const T &exp) {
    if (exp) {
      std::println(">>>>>>>>>>>");
      std::println("NEQ - FALSE");
      auto *st = backtrace_create_state(nullptr, 0, backtrace_err, nullptr);
      backtrace_full(st, 0, backtrace_full, backtrace_err, nullptr);
      std::println("<<<<<<<<<<<");
      asserts.push_back(false);
    } else {
      asserts.push_back(true);
    }
    return asserts.back();
  };

  void dump(std::invocable<const benchmark &> auto &&bm) {
    for (auto &&benchmark : benches) {
      bm(benchmark);
    }
  };

  void dump(std::invocable<const assert &> auto &&st) {
    for (auto &&assert : asserts) {
      st(assert);
    };
  };

  void dump(std::invocable<const assert &> auto &&st,
            std::invocable<const benchmark &> auto &&bm,
            std::invocable<> auto &&separator) {
    for (auto &&assert : asserts) {
      st(assert);
    };
    separator();
    for (auto &&benchmark : benches) {
      bm(benchmark);
    }
  };

  void dump(std::invocable<> auto &&separotor = []() {
    std::println("-------------------");
  }) {
    constexpr auto base_1 = [](const assert &assert) {
      if (assert.has_assert_msg()) {
        std::println("{} - msg : {}", assert ? "[TRUE] " : "[FALSE]",
                     assert.assert_msg());
      } else {
        std::println("{}", assert ? "[TRUE] " : "[FALSE]");
      }
    };

    constexpr auto base_2 = [](const benchmark &benchmark) {
      std::println("[{}] - time : {}|{}|{}", benchmark.get_name(),
                   benchmark.get_lower(), benchmark.get_middle(),
                   benchmark.get_upper());
    };

    dump(base_1, base_2, std::forward<decltype(separotor)>(separotor));
  };

  template <std::size_t N = 1> benchmark &bench(std::invocable<> auto &&call) {
    std::vector<benchmark::value_t> result;
    for (std::size_t i{0}; i < N; ++i) {
      auto begin = benchmark::time_point::clock::now();
      call();
      auto end = benchmark::time_point::clock::now();
      result.push_back({end - begin});
    }

    benchmark::value_t l{100000000000000000};
    benchmark::value_t u{0};
    benchmark::value_t m{0};

    m = std::accumulate(result.begin(), result.end(), benchmark::value_t{0}) /
        result.size();

    for (auto &&r : result) {
      if (r < l) {
        l = r;
      }
      if (r > u) {
        u = r;
      }
    }

    benches.push_back({l, m, u});

    return benches.back();
  };

private:
  std::vector<assert> asserts;
  std::vector<benchmark> benches;
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
        std::println("----------BODY-----------");
        utils u;
        try {
          test.body(u);
        } catch (...) {
          // do
        }
        std::println("---------ASSERTS---------");
        u.dump([]() { std::println("----------BENCH----------"); });
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
