// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.test;
import std;

namespace iuic::test {

namespace complete {

namespace assert {
enum class result {
  Passed,
  Failed,
  // ...
};

struct report {
  // ...
  virtual ~report() {};

  virtual std::string_view msg() const = 0;

  result value() const { return value_; };

  report(result v_) : value_{v_} {}

private:
  result value_;
};
}; // namespace assert

struct execution {

  template <typename T>
    requires std::is_base_of_v<assert::report, T>
  void append(T &&report) {
    if (report.value() == assert::result::Failed) {
      result = false;
    }
    reports.emplace_back(new std::remove_cvref_t<T>{std::forward<T>(report)});
  };

  void log() {
    std::println("[EXECUTION-BEGIN]");
    for (auto &report : reports) {
      if (report->value() == assert::result::Failed) {
        std::println("{}", report->msg());
      }
    }
    std::println("[EXECUTION-END]");
  };

  operator bool() const { return result; };

private:
  bool result{true};
  std::vector<std::unique_ptr<assert::report>> reports;
};

struct run {

  run(std::string_view name_) : name{name_} {}

  void append(execution &&exec) {
    if (not exec) {
      general_result = false;
    }
    executions.emplace_back(std::move(exec));
  };

  void clear() {
    executions.clear();
    general_result = true;
  };

  void log() {
    std::println("[TEST-BEGIN] : {}", name);

    for (auto &exec : executions) {
      if (not exec) {
        exec.log();
      }
    }

    std::println("[TEST-END] : {}", name);
  };

  operator bool() const { return general_result; };

private:
  std::string name{"Unnamed"};
  bool general_result{true};
  std::vector<execution> executions;
};

}; // namespace complete

struct result {
  std::string out;
  std::string msg;
  bool value{true};
};

struct basic_report : complete::assert::report {

  basic_report(std::string_view name, std::string_view msg,
               std::source_location loc, complete::assert::result result)
      : complete::assert::report{result} {
    std::stringstream ss;

    ss << std::endl;
    ss << std::format("[ASSERT-BEGIN] : {}", name);
    ss << std::endl;
    ss << std::format("[RESULT]       : {}",
                      result == complete::assert::result::Passed ? "PASSED"
                                                                 : "FAILED");
    ss << std::endl;
    ss << std::format("[LOCATION]     : {}:{}:{}", loc.file_name(), loc.line(),
                      loc.column());
    ss << std::endl;
    ss << std::format("[MESSAGE]      : {}", msg);
    ss << std::endl;
    ss << std::format("[ASSERT-END]   : {}", name);
    ss << std::endl;

    data = ss.str();
  };

  std::string_view msg() const override { return data; };

private:
  std::string data;
};

export struct utils {
  utils(complete::execution &exec_) : exec{exec_} {}

  void rq_true(bool value, std::string_view msg,
               std::source_location loc = std::source_location::current()) {

    exec.append(basic_report{"RQ_TRUE", msg, loc,
                             value ? complete::assert::result::Passed
                                   : complete::assert::result::Failed});
  };

  void
  rq_true_then(bool value, std::string_view msg, std::invocable<> auto &&call,
               std::source_location loc = std::source_location::current()) {
    exec.append(basic_report{"RQ_TRUE", msg, loc,
                             value ? complete::assert::result::Passed
                                   : complete::assert::result::Failed});

    if (value) {
      call();
    }
  };

  template <typename T>
  void ex_catch(std::invocable<> auto &&call, std::string_view msg,
                std::source_location loc = std::source_location::current()) {
    complete::assert::result result{complete::assert::result::Failed};

    try {
      call();
    } catch (T) {
      result = complete::assert::result::Passed;
      return;
    } catch (...) {
      result = complete::assert::result::Failed;
    };

    exec.append(basic_report{"EX_CATCH", "", loc, result});
  };

  void ex_nothrow(std::invocable<> auto &&call, std::string_view msg,
                  std::source_location loc = std::source_location::current()) {
    complete::assert::result result{complete::assert::result::Passed};

    try {
      call();
    } catch (...) {
      result = complete::assert::result::Failed;
    };

    exec.append(basic_report{"EX_NOTHROW", "", loc, result});
  };

  void rq_false(bool value, std::string_view msg,
                std::source_location loc = std::source_location::current()) {

    exec.append(basic_report{"RQ_FALSE", msg, loc,
                             value ? complete::assert::result::Failed
                                   : complete::assert::result::Passed});
  };

  void
  rq_false_then(bool value, std::string_view msg, std::invocable<> auto &&call,
                std::source_location loc = std::source_location::current()) {

    exec.append(basic_report{"RQ_FALSE", msg, loc,
                             value ? complete::assert::result::Failed
                                   : complete::assert::result::Passed});
    if (value) {
      call();
    }
  };

  template <typename T, typename U>
  void eq(const T &exp, const U &dir, std::string_view msg,
          std::source_location loc = std::source_location::current()) {

    if (exp == dir) {
      exec.append(
          basic_report{"EQ", msg, loc, complete::assert::result::Passed});
    } else {
      exec.append(
          basic_report{"EQ", msg, loc, complete::assert::result::Failed});
    }
  };

  template <typename T, typename U>
  void eq_then(const T &exp, const U &dir, std::string_view msg,
               std::invocable<> auto &&call,
               std::source_location loc = std::source_location::current()) {
    if (exp != dir) {
      exec.append(
          basic_report{"EQ", msg, loc, complete::assert::result::Failed});
    } else {
      exec.append(
          basic_report{"EQ", msg, loc, complete::assert::result::Passed});
      call();
    }
  };

  template <typename T, typename U>
  void neq(const T &exp, const U &dir, std::string_view msg,
           std::source_location loc = std::source_location::current()) {

    if (exp != dir) {
      exec.append(
          basic_report{"NEQ", msg, loc, complete::assert::result::Passed});
    } else {

      exec.append(
          basic_report{"NEQ", msg, loc, complete::assert::result::Failed});
    }
  };

  template <typename T, typename U>
  void neq_than(const T &exp, const U &dir, std::string_view msg,
                std::invocable<> auto &&call,
                std::source_location loc = std::source_location::current()) {

    if (exp != dir) {
      exec.append(
          basic_report{"NEQ", msg, loc, complete::assert::result::Passed});
    } else {

      exec.append(
          basic_report{"NEQ", msg, loc, complete::assert::result::Failed});
    }
  };

private:
  complete::execution &exec;
};

export struct handle {};

export template <typename T> struct usecase;

struct registry {
  struct pkg_test {
    std::string_view name;
    void (*body)(utils);
    std::size_t execute_times{1};
  };

  bool run(bool only_failure = true) {
    if (tests.empty()) {
      return 0;
    }

    std::size_t counter{0};
    std::println("[IUIC] : TEST RUNNER BEGIN");
    for (auto &test : tests) {
      runs.push_back({test.name});
      auto &run = runs.back();

      std::println("[EXECUTE-TIMES] : {}", test.execute_times);
      for (std::size_t i{0}; i < test.execute_times; ++i) {
        complete::execution exec;

        test.body(utils{exec});
        std::println("----");
        run.append(std::move(exec));
      }
      std::println("--------");
    }
    std::println("[IUIC] : TEST RUNNER END");

    for (auto &run : runs) {
      if (not run) {
        return false;
      }
    }
    return true;
  };

  void log() {
    std::println("[IUIC] : TEST LOGGER BEGIN");

    for (auto &run : runs) {
      if (not run) {
        run.log();
      }
    }

    std::println("[IUIC] : TEST LOGGER END");
  };

  static registry &instance() {
    static constinit registry _{};
    return _;
  };

  template <typename T> handle registry_test() {
    pkg_test test;

    test.name = T::name;
    test.body = T::body;

    if constexpr (requires {
                    { T::execute_times } -> std::convertible_to<std::size_t>;
                  }) {
      test.execute_times = T::execute_times;
    } else {
      test.execute_times = 1;
    }

    tests.push_back(test);
    return {};
  };

  std::vector<pkg_test> tests;
  std::vector<complete::run> runs;
};

export template <typename T> struct unit {

  unit() { handle_touch(); };

private:
  void handle_touch() {
    static handle handle = registry::instance().template registry_test<T>();
  };
};

export int run(bool force_log = false) {
  auto &instance = registry::instance();

  if (instance.run()) {
    if (force_log) {
      instance.log();
    }
    return 0;
  } else {
    instance.log();
    return 1;
  }
};

}; // namespace iuic::test
