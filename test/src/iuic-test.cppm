// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.test;
import std;

namespace iuic::test {

struct result {
  std::string out;
  std::string msg;
  bool value{true};
};

export struct utils {
  utils(result &result_) : result{result_} {}

  void rq_true(bool value, std::string_view msg,
               std::source_location loc = std::source_location::current()) {
    if (not value) {
      result.msg += std::format("[RQ_TRUE(FAILED)] --- {} --- ", msg);
      result.msg += '\n';
      result.msg += '\n';

      auto fmt_location = std::format("location : {}:{}:{}", loc.file_name(),
                                      loc.line(), loc.column());
      result.msg += fmt_location;
      result.msg += '\n';
      result.msg += '\n';
      result.msg += std::format("[RQ_TRUE(FAILED)] --- {} --- ", msg);

      result.value = false;
    }
  };

  template <typename T>
  void ex_catch(std::invocable<> auto &&call, std::string_view msg,
                std::source_location loc = std::source_location::current()) {
    try {
      call();
    } catch (T) {
      return;
    } catch (...) {
    };

    result.msg += std::format("[EX_CATCH(FAILED)] --- {} --- ", msg);
    result.msg += '\n';
    result.msg += '\n';

    auto fmt_location = std::format("location : {}:{}:{}", loc.file_name(),
                                    loc.line(), loc.column());
    result.msg += fmt_location;
    result.msg += '\n';
    result.msg += '\n';
    result.msg += std::format("[EX_CATCH(FAILED)] --- {} --- ", msg);

    result.value = false;
  };

  void ex_nothrow(std::invocable<> auto &&call, std::string_view msg,
                  std::source_location loc = std::source_location::current()) {
    try {
      call();
    } catch (...) {
      result.msg += std::format("[EX_CATCH(FAILED)] --- {} --- ", msg);
      result.msg += '\n';
      result.msg += '\n';

      auto fmt_location = std::format("location : {}:{}:{}", loc.file_name(),
                                      loc.line(), loc.column());
      result.msg += fmt_location;
      result.msg += '\n';
      result.msg += '\n';
      result.msg += std::format("[EX_CATCH(FAILED)] --- {} --- ", msg);

      result.value = false;
    };
  };

  void rq_false(bool res, std::string_view msg,
                std::source_location loc = std::source_location::current()) {
    if (res) {
      result.msg += std::format("[RQ_FALSE(FAILED)] --- {} --- ", msg);
      result.msg += '\n';
      result.msg += '\n';

      auto fmt_location = std::format("location : {}:{}:{}", loc.file_name(),
                                      loc.line(), loc.column());
      result.msg += fmt_location;
      result.msg += '\n';
      result.msg += '\n';
      result.msg += std::format("[RQ_FALSE(FAILED)] --- {} --- ", msg);

      result.value = false;
    }
  };

  template <typename T, typename U>
  void eq(const T &exp, const U &dir, std::string_view msg,
          std::source_location loc = std::source_location::current()) {
    if (exp != dir) {
      result.msg += std::format("[QE(FAILED)] --- {} --- ", msg);
      result.msg += '\n';
      result.msg += '\n';

      auto fmt_location = std::format("location : {}:{}:{}", loc.file_name(),
                                      loc.line(), loc.column());
      result.msg += fmt_location;
      result.msg += '\n';
      result.msg += '\n';
      result.msg += std::format("[QE(FAILED)] --- {} --- ", msg);

      result.value = false;
    }
  };

  template <typename T, typename U>
  void neq(const T &exp, const U &dir, std::string_view msg,
           std::source_location loc = std::source_location::current()) {
    if (exp == dir) {
      result.msg += std::format("[NQE(FAILED)] --- {} --- ", msg);
      result.msg += '\n';
      result.msg += '\n';

      auto fmt_location = std::format("location : {}:{}:{}", loc.file_name(),
                                      loc.line(), loc.column());
      result.msg += fmt_location;
      result.msg += '\n';
      result.msg += '\n';
      result.msg += std::format("[NQE(FAILED)] --- {} --- ", msg);

      result.value = false;
    }
  };

private:
  result &result;
};

export struct handle {};

export template <typename T> struct usecase;

export struct registry {
  struct pkg_test {
    std::string name;
    std::function<void(utils)> body;
  };

  int run(bool only_failure = true) {
    std::size_t counter{0};
    std::println("[IUIC] : TEST RUNNER BEGIN");
    for (auto &test : tests) {

      result.push_back({});
      test.body(utils{result.back()});
      if (result.back().value) {
        ++counter;
      }

      if (only_failure && result.back().value) {
        continue;
      }
      std::println("[TEST] : --- {} ---",
                   test.name.empty() ? "UNNAMED" : test.name);
      std::println();

      std::println("{}", result.back().msg);

      std::println();
      std::println("[TEST({})] : --- {} ---",
                   result.back().value ? "OK" : "FAILED",
                   test.name.empty() ? "UNNAMED" : test.name);
      std::println();
    }

    std::println("[IUIC] : TEST RUNNER END");
    std::println("[IUIC] : {}/{} [{}]", counter, tests.size(),
                 counter == tests.size() ? "PASSED" : "FAILED");

    return counter == tests.size() ? 0 : 1;
  };

  static registry &instance() {
    static registry _;
    return _;
  };

  template <typename T> handle registry_test() {
    tests.push_back(
        pkg_test{.name = std::string{T::name},
                 .body = [](utils utils) { T{}.body(std::move(utils)); }});
    return {};
  };

  std::vector<pkg_test> tests;
  std::vector<result> result;
};

export template <typename T> struct unit {

  unit() { handle_touch(); };

private:
  void handle_touch() {
    static handle handle = registry::instance().template registry_test<T>();
  };
};

export int run(bool only_failure = false) {
  return registry::instance().run(only_failure);
};

}; // namespace iuic::test
