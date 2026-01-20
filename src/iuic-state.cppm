// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:state;
import std;
import iuic.underlying;

export namespace iuic {

struct state final {
  struct decl;
  struct base;
  struct machine;

  state() = delete;

  constexpr state(const decl *state_ptr) : value{state_ptr} {};
  constexpr state(const decl &state_ref) : value{&state_ref} {};

  constexpr bool operator==(const state &other) const noexcept {
    return (std::size_t)other.value == (std::size_t)value;
  };

  operator std::size_t() const noexcept { return (std::size_t)value; };

  constexpr bool operator!=(const state &other) const noexcept {
    return not(other.value == value);
  };

  constexpr state &operator=(const state &other) noexcept {
    value = other.value;
    return *this;
  };

  const decl *value{nullptr};
};

struct state::decl final {

  decl(const decl &) = delete;
  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  constexpr operator state() noexcept { return {this}; }

  // force static\global
  const decl *self{this};

  // constexpr static unique instance
  // use for unique 'state::decl' pointer address
  template <typename... UNIQUE_GUARD, typename UNIQUE = decltype([]() {
                                        struct _ {};
                                        return _{};
                                      }())>
  static constexpr const decl &unique_instance() {
    return unique_instance_impl(UNIQUE{});
  };

private:
  consteval decl() noexcept {};

  template <typename UNIQUE>
  static constexpr const decl &unique_instance_impl(UNIQUE unique) {
    static constexpr state::decl _{};
    return _;
  };
};

struct state::base {
  static constexpr auto &null{decl::unique_instance()};

  static constexpr auto &hovered{decl::unique_instance()};

  static constexpr auto &idle{decl::unique_instance()};

  static constexpr auto &err{decl::unique_instance()};
};

/* -- Прерывание
когда нужно выпонить прерывания состояния\перехода, посылается
специальный сигнал. В этом случае корутина должна возвращать
отчет о прирываний [process,sucess,err]. Процесс прерывания
может длиться долго, а переход в новое состояние будет только
после [sucess].
*/
struct state::machine {

  template <iuic::state from, iuic::state to, bool bidirectional = false>
  struct transition {
    static constexpr std::pair value{from, to};
    static constexpr bool is_bidirectional{bidirectional};
  };

  template <transition entry_, transition... trs_> struct transition_graph {
    static constexpr auto entry = entry_;

    static constexpr std::tuple value{trs_...};

    static constexpr std::size_t transition_count = []() { return 0; }();

    static constexpr std::size_t invalid_index{
        std::numeric_limits<std::size_t>::max()};

    static constexpr std::size_t stay_count = []() { return 0; }();

    static constexpr std::size_t transition_index(state, state);

    static constexpr std::size_t stay_index(state);
  };

  struct control_block {

    virtual ~control_block() = default;

    virtual constexpr bool can_move(iuic::state) const = 0;

    virtual constexpr bool try_move(iuic::state) = 0;

    virtual constexpr bool force_move(iuic::state) = 0;

    virtual constexpr void rethrow() const = 0;

    // visit shared
    template <typename T>
    constexpr bool try_visit_shared(
        erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call) {
      get_shared_object().try_visit(std::forward<decltype(call)>(call));
    };

    template <typename T>
    constexpr bool try_visit_shared(
        erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call)
        const {
      get_shared_object().try_visit(std::forward<decltype(call)>(call));
    };

  protected:
    virtual erasure::visited::as_mutable get_shared_object() = 0;
    virtual erasure::visited::as_const get_shared_object() const = 0;
  };

  struct machine_block : control_block {
    virtual ~machine_block() = default;

    virtual constexpr void process() = 0;

    virtual constexpr std::size_t spec_id() const = 0;
  };

  struct execute {
    execute() = delete ("Use just for namespace");

    struct state {
      iuic::state from{iuic::state::base::idle};

      union {
        iuic::state to{iuic::state::base::idle}; // also use by  : stay
        iuic::state active;                      // alt name for : stay
      };

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

      using enum value_t;

      constexpr operator value_t() const noexcept { return value; }
    };

    enum class result {
      process,
      process_interrupt,
      success_interrupt,
      err,
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

        std::suspend_always yield_value(result) { return {}; };

        void return_void() {};

        void unhandled_exception() {};

      private:
      };

      void process() {
        if (not handle.done())
          handle.resume();
      };

      transition(promise_type::handle_type handle_) : handle{handle_} {};

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

        std::suspend_always yield_value(result) { return {}; };

        // возврат невозможного перехода == UB
        void return_value(iuic::state){};

        void unhandled_exception() {};

      private:
      };

      void process() {
        if (not handle.done())
          handle.resume();
      };

      stay(promise_type::handle_type handle_) : handle{handle_} {};

      ~stay() {
        if (handle)
          handle.destroy();
      };

    private:
      iuic::state selected{iuic::state::base::null};
      promise_type::handle_type handle;
    };
  };

  template <transition_graph graph, iuic::erasure::as_pure_type SharedData>
  struct spec {
    using shared_type = SharedData;

    struct prototype {
      using spec = spec;
      using stay_ctor_map =
          std::array<execute::stay::ctor<SharedData>, graph.transition_count>;
      using transition_ctor_map =
          std::array<execute::transition::ctor<SharedData>,
                     graph.transition_count>;

      execute::transition::ctor<SharedData>
      get_transition_ctor(state from, state to) const {
        if (auto index = graph.transition_index(from, to);
            index != graph.invalid_index) {
          return tr_[index];
        }

        return nullptr;
      };

      execute::stay::ctor<SharedData> get_stay_ctor(state stay) const {
        if (auto index = graph.stay_index(stay); index != graph.invalid_index) {
          return st_[index];
        }

        return nullptr;
      };

      constexpr prototype(transition_ctor_map tr, stay_ctor_map st)
          : tr_{tr}, st_{st} {};

      static constexpr execute::transition::ctor<SharedData>
          default_transition_ctor =
              [](const execute::state &state,
                 SharedData &) static -> execute::transition {
        if (state.is_interrupted()) {
          co_yield execute::result::success_interrupt;
        }
        co_return;
      };

      static constexpr execute::stay::ctor<SharedData> default_stay_ctor =
          [](const execute::state &state,
             SharedData &) static -> execute::stay {
        for (; not state.is_interrupted();) {
          co_yield execute::result::process;
        }

        if (state.is_interrupted()) {
          co_yield execute::result::success_interrupt;
        }

        co_return iuic::state::base::null;
      };

    private:
      transition_ctor_map tr_;
      stay_ctor_map st_;
    };

    struct protobuilder {

      constexpr void stay(iuic::state stay,
                          execute::stay::ctor<SharedData> stay_ctor) {
        if (auto index = graph.stay_index(stay); index != graph.invalid_index) {
          st_[index] = stay_ctor;
        }
      };

      constexpr void
      transition(iuic::state from, iuic::state to,
                 execute::transition::ctor<SharedData> transition_ctor) {
        if (auto index = graph.transition_index(from, to);
            index != graph.invalid_index) {
          tr_[index] = transition_ctor;
        }
      };

      constexpr prototype finalize() const noexcept { return {tr_, st_}; };

      constexpr operator prototype() const noexcept { return finalize(); };

    private:
      prototype::transition_ctor_map tr_{[]() static constexpr {
        return
            []<std::size_t... i>(std::index_sequence<i...>) static constexpr {
              return typename prototype::transition_ctor_map{
                  ((void)i, prototype::default_transition_ctor)...};
            }(std::make_index_sequence<graph.transition_count>{});
      }()};

      prototype::stay_ctor_map st_{[]() static constexpr {
        return
            []<std::size_t... i>(std::index_sequence<i...>) static constexpr {
              return typename prototype::stay_ctor_map{
                  ((void)i, prototype::default_stay_ctor)...};
            }(std::make_index_sequence<graph.stay_count>{});
      }()};
    };

    static constexpr protobuilder get_protobuilder() { return {}; };

    struct machine_block : machine::machine_block {
      // TOTO : make normal ctor
      machine_block(const prototype &p) : prototype{p} {};

      constexpr bool can_move(iuic::state) const override{};

      // выставляет следующее состояние
      constexpr bool try_move(iuic::state nstate) override {
        if (can_move(nstate)) {
          // DO MOVE
          auto ctor = prototype.get_transition_ctor(state.to, nstate);

          auto handle = ctor(state, data);

          return true;
        }

        return false;
      };

      // если сейча осуществляется прерход
      // делает прерывание и выставляет следующее состояние
      constexpr bool force_move(iuic::state nstate) override {
        if (can_move(nstate)) {
        }

        return false;
      };

      constexpr void process() override {
        //
      };

      constexpr std::size_t spec_id() const override {};

      constexpr void rethrow() const override {};

      erasure::visited::as_mutable get_shared_object() override {
        return erasure::visited::as_mutable{std::addressof(data)};
      };

      erasure::visited::as_const get_shared_object() const override {
        return erasure::visited::as_const{std::addressof(data)};
      };

    private:
      iuic::state selected{iuic::state::base::null};
      const prototype &prototype;
      SharedData data;
      execute::state state{iuic::state::base::idle};
      execute::transition tr_handler{nullptr};
      execute::stay st_handler{nullptr};
    };

    static constexpr prototype make_prototype(auto... state_handler) {
      return {};
    };

    static constexpr machine_block make_machine_block(const prototype &proto) {
      return machine_block{proto};
    };
  };

  // стуктура управляющая
  // всеми машинами
  struct hub {
    // у хаба должна быть своя память под машины

    // Есть активный вопрос
    // в кокой момет исполнения
    // ctx.make();
    // выполняеться этот метод ?
    // начало(фафорит), где-то в промежутке или в конце
    void execute();

    machine_block *get_machine(iuic::units::uid);

    const machine_block *get_machine(iuic::units::uid) const;

    // init or update livetime
    bool machine_instance(iuic::units::uid, const auto &prototype);
  };
};

}; // namespace iuic

namespace std {
export template <> struct std::hash<iuic::state> {
  std::size_t operator()(const iuic::state &s) const noexcept { return s; }
};

}; // namespace std

iuic::state::machine::execute::stay
foo(const iuic::state::machine::execute::state &state, int &data) {

  auto heh = [](const iuic::state::machine::execute::state &state,
                int &data) -> iuic::state::machine::execute::transition {
    for (; not state.is_interrupted();) {
      // do jobs
      co_yield iuic::state::machine::execute::result::process;
    }
    co_return;
  };

  for (; not state.is_interrupted();) {
    // do job
    co_yield iuic::state::machine::execute::result::process;
  }

  if (state.is_interrupted()) {
    co_yield iuic::state::machine::execute::result::success_interrupt;
  }

  co_return iuic::state::base::idle;
}

void oaeu() {
  using fptr = void (*)() noexcept;

  fptr a = []() static noexcept {};
};

void heh() {
  static auto &a{iuic::state::decl::unique_instance()};

  using execute = iuic::state::machine::execute;

  auto tr = [](const execute::state &state, int &) -> execute::stay {
    for (; not state.is_interrupted();) {

      try {
      } catch (...) {
        co_return iuic::state::base::err;
      };

      co_yield execute::result::process;
    }

    co_return iuic::state::base::idle;
  };
};
