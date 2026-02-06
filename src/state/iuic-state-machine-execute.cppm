export module iuic.state:machine.execute;
import std;
import :value;
import :base;
import :machine.exception;

export namespace iuic::state::machine::execute {

struct state {
  iuic::state::value from{iuic::state::base::idle};

  union {
    iuic::state::value to{iuic::state::base::idle}; // also use by  : stay
    iuic::state::value active;                      // alt name for : stay
  };

  std::exception_ptr exception{nullptr};

  enum class value_t {
    idle,
    transition,
    transition_interrupt,
    stay,
    stay_interrupt,
    err,
  } value{idle};

  constexpr bool is_interrupted() const noexcept {
    return value == transition_interrupt || value == stay_interrupt;
  };

  constexpr void reverse() { std::swap(from, to); };

  using enum value_t;

  constexpr operator value_t() const noexcept { return value; }
};

enum class result {
  process,
  process_interrupt,
  success_interrupt,
  err,
  terminate,
  terminate_by_unexpected_exception,
};

// [from -> to]
struct transition {
  template <typename Shared>
  using ctor = transition (*)(const state &, Shared &);

  struct promise_type {
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type get_return_object() {
      return handle_type::from_promise(*this);
    };

    std::suspend_always initial_suspend() noexcept { return {}; };

    std::suspend_always final_suspend() noexcept { return {}; };

    std::suspend_always yield_value(result res) {
      last_yield = res;
      return {};
    };

    void return_void() {};

    void unhandled_exception() { exception = std::current_exception(); };

    result last_yield{result::process};
    std::exception_ptr exception{nullptr};
  };

  constexpr transition(transition &&other) noexcept {
    std::swap(handle, other.handle);
  };

  constexpr transition &operator=(transition &&other) noexcept {
    std::swap(handle, other.handle);
    return *this;
  };

  transition(const transition &) = delete;
  transition &operator=(const transition &) = delete;
  transition(promise_type::handle_type handle_) : handle{handle_} {};
  explicit transition(std::nullptr_t) : handle{nullptr} {};

  void process() {
    if (handle && not handle.done())
      handle.resume();
  };

  const std::exception_ptr &exception() const {
    static std::exception_ptr _{};
    if (handle) {
      return handle.promise().exception;
    } else {
      _ = std::make_exception_ptr(weak_machine{});
      return _;
    }
  };

  bool finished() const { return not handle || handle.done(); };

  execute::result last_yield() const { return handle.promise().last_yield; };

  ~transition() {
    if (handle)
      handle.destroy();
  };

private:
  promise_type::handle_type handle;
};

// [current]
struct stay {
  template <typename Shared> using ctor = stay (*)(const state &, Shared &);

  struct promise_type {
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type get_return_object() {
      return handle_type::from_promise(*this);
    };

    std::suspend_always initial_suspend() noexcept { return {}; };

    std::suspend_always final_suspend() noexcept { return {}; };

    std::suspend_always yield_value(result ret) {
      last_yield = ret;
      return {};
    };

    // возврат невозможного перехода == UB
    void return_value(iuic::state::value ret) {
      if (ret == iuic::state::base::null) {
        result = iuic::state::base::idle;
      } else {
        result = ret;
      }
    };

    void unhandled_exception() { exception = std::current_exception(); };

    std::exception_ptr exception{nullptr};
    result last_yield{result::process};
    iuic::state::value result{iuic::state::base::null};
  };

  constexpr stay(stay &&other) noexcept { std::swap(handle, other.handle); };

  constexpr stay &operator=(stay &&other) noexcept {
    std::swap(handle, other.handle);
    return *this;
  };

  stay(const stay &) = delete;
  stay &operator=(const stay &) = delete;
  stay(promise_type::handle_type handle_) : handle{handle_} {};
  explicit stay(std::nullptr_t) : handle{nullptr} {};

  void process() {
    if (handle && not handle.done())
      handle.resume();
  };

  bool finished() const { return not handle || handle.done(); };

  execute::result last_yield() const { return handle.promise().last_yield; };

  iuic::state::value return_value() const { return handle.promise().result; };

  const std::exception_ptr &exception() const {
    static std::exception_ptr _{};
    if (handle) {
      return handle.promise().exception;
    } else {
      _ = std::make_exception_ptr(weak_machine{});
      return _;
    }
  };

  ~stay() {
    if (handle)
      handle.destroy();
  };

private:
  promise_type::handle_type handle;
};
}; // namespace iuic::state::machine::execute
