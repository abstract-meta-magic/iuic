// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:state;
import std;
import iuic.underlying;

namespace iuic::state {

export struct decl;

export struct value final {

  constexpr value() : decl{nullptr} {};

  constexpr value(const decl *state_ptr) : decl{state_ptr} {};
  constexpr value(const decl &state_ref) : decl{&state_ref} {};

  constexpr bool operator==(const value &other) const noexcept {
    return other.decl == decl;
  };

  operator std::size_t() const noexcept { return (std::size_t)decl; };

  constexpr bool operator!=(const value &other) const noexcept {
    return not(other.decl == decl);
  };

  constexpr value &operator=(const value &other) noexcept {
    decl = other.decl;
    return *this;
  };

  const decl *decl{nullptr};
};

struct decl final {

  decl(const decl &) = delete;
  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  constexpr operator value() noexcept { return {this}; }

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
    static constexpr decl _{};
    return _;
  };
};

export namespace base {
constexpr auto &null{decl::unique_instance()};

constexpr auto &hovered{decl::unique_instance()};

constexpr auto &idle{decl::unique_instance()};

constexpr auto &exception_marshaling{decl::unique_instance()};

constexpr auto &terminate{decl::unique_instance()};
}; // namespace base

/* -- Прерывание
когда нужно выпонить прерывания состояния\перехода, посылается
специальный сигнал. В этом случае корутина должна возвращать
отчет о прирываний [process,sucess,err]. Процесс прерывания
может длиться долго, а переход в новое состояние будет только
после [sucess].
*/
namespace machine {

export struct transition {
  value from;
  value to;
  bool is_bidirectional{false};
  constexpr bool operator==(const transition &other) const {
    return from == other.from && to == other.to;
  };
};

template <transition value> struct help_t {
  static constexpr auto get() {
    return []<transition... trs>(utils::ct::list<transition, trs...>) {
      if constexpr (sizeof...(trs) < 1) {
        return utils::ct::list<transition, value>{};
      } else if constexpr ((false || ... ||
                            (trs.from == value.from && trs.to == value.to))) {
        return utils::ct::list<transition, trs...>{};
      } else {
        return utils::ct::list<transition, trs..., value>{};
      }
    };
  };
};

template <value val> struct help_s {
  static constexpr auto get() {
    return []<value... trs>(utils::ct::list<value, trs...>) {
      if constexpr (sizeof...(trs) < 1) {
        return utils::ct::list<value, val>{};
      } else if constexpr ((false || ... || (trs == val))) {
        return utils::ct::list<value, trs...>{};
      } else {
        return utils::ct::list<value, trs..., val>{};
      }
    };
  };
};

template <transition... val> consteval auto make_stay_index_tree() {
  static constexpr auto expand =
      (utils::ct::list<value>{} & ... &
       (utils::ct::list<value, val.from>{} & utils::ct::list<value, val.to>{}));

  static constexpr auto remove_dublicate = expand.expand([]<value... list>() {
    return (utils::ct::list<value>{} & ... & (help_s<list>::get()));
  });

  return utils::ct::index_tree{remove_dublicate.expand(
      []<value... list>() { return std::array{list...}; })};
};

template <transition... value> consteval auto make_transition_index_tree() {

  static constexpr auto expand =
      ((utils::ct::list<transition, value>{} &
        []<transition inner>(utils::ct::list<transition, inner>) {
          if constexpr (inner.is_bidirectional) {
            return utils::ct::list<transition, inner,
                                   transition{inner.to, inner.from}>{};
          } else {
            return utils::ct::list<transition, inner>{};
          }
        }) &
       ...);

  static constexpr auto remove_dublicate =
      []<transition... list>(utils::ct::list<transition, list...>) {
        return (utils::ct::list<transition>{} & ... & help_t<list>::get());
      }(expand);

  return utils::ct::index_tree{remove_dublicate.expand(
      []<transition... list>() { return std::array{list...}; })};
};

export template <transition... trs_> struct transition_graph {
  // TODO : нужна валидация графа
  static constexpr auto transition_index_tree =
      make_transition_index_tree<trs_...>();

  static constexpr auto stay_index_tree = make_stay_index_tree<trs_...>();

  static constexpr std::tuple value{trs_...};

  static constexpr std::size_t transition_count = transition_index_tree.size();

  static constexpr std::size_t stay_count = stay_index_tree.size();

  static constexpr std::size_t invalid_index{
      std::numeric_limits<std::size_t>::max()};

  static constexpr std::size_t transition_index(state::value from,
                                                state::value to) {
    return transition_index_tree.get_index({from, to});
  };

  static constexpr std::size_t stay_index(state::value stay) {
    return stay_index_tree.get_index(stay);
  };
};

export namespace execute {
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

    std::suspend_always yield_value(result) { return {}; };

    void return_void() {};

    void unhandled_exception() {};

  private:
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
    void return_value(iuic::state::value){};

    void unhandled_exception() {};

  private:
  };

  constexpr stay(stay &&other) noexcept {
    std::swap(selected, other.selected);
    std::swap(handle, other.handle);
  };

  constexpr stay &operator=(stay &&other) noexcept {
    std::swap(selected, other.selected);
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

  ~stay() {
    if (handle)
      handle.destroy();
  };

private:
  iuic::state::value selected{iuic::state::base::null};
  promise_type::handle_type handle;
};
}; // namespace execute

struct controller {
  virtual bool can_move(state::value) const = 0;

  virtual bool try_move(state::value) = 0;

  virtual bool force_move(state::value) = 0;

  virtual void rethrow() const = 0;

  // visit shared
  constexpr bool try_visit_shared(
      erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call) {
    return get_shared_object().try_visit(std::forward<decltype(call)>(call));
  };

  constexpr bool try_visit_shared(
      erasure::func_as_decoy<erasure::decoy(const erasure::decoy &)> auto
          &&call) const {
    return get_shared_object().try_visit(std::forward<decltype(call)>(call));
  };

protected:
  virtual erasure::visited::as_mutable get_shared_object() = 0;
  virtual erasure::visited::as_const get_shared_object() const = 0;
  ~controller() = default;
};

struct instance {
  virtual ~instance() = default;

  virtual controller &get_controller() = 0;

  virtual const execute::state &get_execute_state() const = 0;

  virtual std::size_t get_spec_id() const = 0;

  virtual void process() = 0;
};

// TODO : DOCS
export template <transition_graph graph, iuic::erasure::as_pure_type SharedData>
struct spec {
  using shared_type = SharedData;

private:
  struct prototype_base {
    using spec = spec;
    using stay_ctor_t = execute::stay::ctor<SharedData>;
    using transition_ctor_t = execute::transition::ctor<SharedData>;
    using stay_ctor_map_t = std::array<stay_ctor_t, graph.transition_count>;
    using transition_ctor_map_t =
        std::array<transition_ctor_t, graph.transition_count>;

    constexpr prototype_base(std::size_t entry_index_, stay_ctor_t excp_ctor,
                             transition_ctor_t term_ctor,
                             transition_ctor_map_t tr_map,
                             stay_ctor_map_t st_map) noexcept
        : entry_index{entry_index_}, exception_handler_ctor{excp_ctor},
          terminate_handler_ctor{}, transition_ctor_map{tr_map},
          stay_ctor_map{st_map} {};

  protected:
    std::size_t entry_index;
    stay_ctor_t exception_handler_ctor;
    transition_ctor_t terminate_handler_ctor;
    transition_ctor_map_t transition_ctor_map;
    stay_ctor_map_t stay_ctor_map;
  };

  struct prototype : public prototype_base {
    prototype_base::transition_ctor_t get_transition_ctor(value from,
                                                          value to) const {
      if (auto index = graph.transition_index(from, to);
          index != graph.invalid_index) {
        return prototype_base::transition_ctor_map[index];
      }

      return nullptr;
    };

    prototype_base::stay_ctor_t get_stay_ctor(value stay) const {
      if (auto index = graph.stay_index(stay); index != graph.invalid_index) {
        return prototype_base::stay_ctor_map[index];
      }

      return nullptr;
    };

    prototype_base::stay_ctor_t get_entry() const {
      return prototype_base::stay_ctor_map[prototype_base::entry_index];
    };

    prototype_base::stay_ctor_t get_exception_handler() const {
      return prototype_base::exception_handler;
    };

    prototype_base::transition_ctor_t get_terminate_handler() const {
      return prototype_base::terminate_handler;
    };
  };

  struct protobuilder : public prototype_base {
  private: // default init
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
        [](const execute::state &state, SharedData &) static -> execute::stay {
      for (; not state.is_interrupted();) {
        co_yield execute::result::process;
      }

      if (state.is_interrupted()) {
        co_yield execute::result::success_interrupt;
      }

      co_return iuic::state::base::null;
    };

    static constexpr execute::transition::ctor<SharedData>
        default_terminate_handler_ctor{
            [](const execute::state &state,
               SharedData &) static -> execute::transition { co_return; }};

    static constexpr execute::stay::ctor<SharedData>
        default_exception_handler_ctor{
            [](const execute::state &state,
               SharedData &) static -> execute::stay {
              if (state.exception) {
                co_yield execute::result::terminate_by_unexpected_exception;
              }
              co_return base::idle;
            }};

    static constexpr prototype_base::transition_ctor_map_t
        default_transition_ctor_map{[]() static constexpr {
          return
              []<std::size_t... i>(std::index_sequence<i...>) static constexpr {
                return typename prototype_base::transition_ctor_map_t{
                    ((void)i, default_transition_ctor)...};
              }(std::make_index_sequence<graph.transition_count>{});
        }()};

    static constexpr prototype_base::stay_ctor_map_t default_stay_ctor_map{
        []() static constexpr {
          return
              []<std::size_t... i>(std::index_sequence<i...>) static constexpr {
                return typename prototype_base::stay_ctor_map_t{
                    ((void)i, default_stay_ctor)...};
              }(std::make_index_sequence<graph.stay_count>{});
        }()};

  public: // API
    constexpr protobuilder() noexcept
        : prototype_base{0, default_exception_handler_ctor,
                         default_terminate_handler_ctor,
                         default_transition_ctor_map, default_stay_ctor_map} {}

    constexpr protobuilder &stay(state::value stay,
                                 execute::stay::ctor<SharedData> stay_ctor) {
      if (auto index = graph.stay_index(stay); index != graph.invalid_index) {
        prototype_base::stay_ctor_map[index] = stay_ctor;
      }

      return *this;
    };

    constexpr protobuilder &
    transition(state::value from, state::value to,
               execute::transition::ctor<SharedData> transition_ctor) {
      if (auto index = graph.transition_index(from, to);
          index != graph.invalid_index) {
        prototype_base::transition_ctor_map[index] = transition_ctor;
      }

      return *this;
    };

    template <state::value val>
    constexpr protobuilder &entry()
      requires(graph.stay_index(val) != graph.invalid_index)
    {
      prototype_base::entry_index = graph.stay_index(val);
      return *this;
    };

    constexpr protobuilder &
    terminate(execute::transition::ctor<SharedData> term_ctor) {
      prototype_base::terminate_handler_ctor = term_ctor;
      return *this;
    };

    constexpr protobuilder &
    exception_handler(execute::stay::ctor<SharedData> init_ctor) {
      prototype_base::exception_handler_ctor = init_ctor;
      return *this;
    };

    constexpr prototype finalize() const noexcept { return {*this}; };

    constexpr operator prototype() const noexcept { return finalize(); };
  };

public:
  static consteval protobuilder get_protobuilder() { return {}; };

  struct instance final : public machine::instance, private controller {
  private: // controller
    bool can_move(state::value) const override {
      // TODO : body
      return false;
    };

    // выставляет следующее состояние
    bool try_move(state::value nstate) override {
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
    constexpr bool force_move(state::value nstate) override {
      if (can_move(nstate)) {
      }

      return false;
    };
    constexpr void rethrow() const override {};

    erasure::visited::as_mutable get_shared_object() override {
      return erasure::visited::as_mutable{std::addressof(data)};
    };

    erasure::visited::as_const get_shared_object() const override {
      return erasure::visited::as_const{std::addressof(data)};
    };

  public: // instance
    instance(const prototype &p) : prototype{p} {
      st_handler = p.get_entry()(state, data);
      std::println("init");
      state.value = execute::state::stay;
    };

    controller &get_controller() override { return *this; };

    const execute::state &get_execute_state() const override { return state; };

    std::size_t get_spec_id() const override { return spec::id; };

    void process() override {
      if (selected != state::base::null) {
        switch (state.value) {
        case execute::state::idle: {
          break;
        }
        case execute::state::stay: {
          st_handler.process();
          break;
        }
        case execute::state::stay_interrupt: {
          break;
        }
        case execute::state::transition: {
          break;
        }
        case execute::state::transition_interrupt: {
          break;
        }
        case execute::state::err: {
          break;
        }
        }

      } else {
        switch (state.value) {
        case execute::state::idle: {
          break;
        }
        case execute::state::stay: {
          st_handler.process();
          break;
        }
        case execute::state::stay_interrupt: {
          break;
        }
        case execute::state::transition: {
          break;
        }
        case execute::state::transition_interrupt: {
          break;
        }
        case execute::state::err: {
          break;
        }
        }
      }
    };

  private:
    const prototype &prototype;
    SharedData data;
    state::value selected{iuic::state::base::null};
    execute::state state{iuic::state::base::idle};
    execute::transition tr_handler{nullptr};
    execute::stay st_handler{nullptr};
  };

  static constexpr std::size_t id{0};

  static constexpr prototype make_prototype(auto... state_handler) {
    return {};
  };

  static constexpr std::unique_ptr<machine::instance>
  make_instance(const prototype &proto) {
    return std::make_unique<instance>(proto);
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
  void execute() {
    for (auto &&[_, machine] : machines) {
      if (machine) {
        // TOTO : normal processing
        machine->process();
      }
    };
  };

  instance *get_machine(iuic::units::uid uid) {
    return machines.contains(uid) ? machines.at(uid).get() : nullptr;
  };

  const instance *get_machine(iuic::units::uid uid) const {
    return machines.contains(uid) ? machines.at(uid).get() : nullptr;
  };

  // init or update livetime
  bool machine_instance(iuic::units::uid uid, const auto &prototype) {
    using spec = typename std::remove_cvref_t<decltype(prototype)>::spec;

    if (not machines.contains(uid)) {
      machines.insert(uid, spec::make_instance(prototype));
    } else if (machines.at(uid)->get_spec_id() != spec::id) {
      machines.at(uid).swap(spec::make_instance(prototype));
    };
  };

private:
  std::map<iuic::units::uid, std::unique_ptr<instance>> machines;
};
}; // namespace machine

}; // namespace iuic::state

namespace std {
export template <> struct std::hash<iuic::state::value> {
  std::size_t operator()(const iuic::state::value &s) const noexcept {
    return s;
  }
};

}; // namespace std

constexpr void test() {
  using namespace iuic::state;
  static auto &a{iuic::state::decl::unique_instance()};
  static auto &b{iuic::state::decl::unique_instance()};
  static auto &c{iuic::state::decl::unique_instance()};

  static constexpr auto tree_t =
      machine::make_transition_index_tree<{a, b, true}, {b, c, true}, {a, b},
                                          {a, b}>();

  static constexpr auto tree_s =
      machine::make_stay_index_tree<{a, b, true}, {b, c, true}, {a, b},
                                    {a, b}>();

  static constexpr std::size_t sz_t = tree_t.size();
  static constexpr std::size_t sz_s = tree_s.size();

  static constexpr std::size_t i_t = tree_t.get_index({c, b});

  struct Anim {};

  using spec = machine::spec<
      machine::transition_graph<machine::transition{a, b, true},
                                machine::transition{b, c, true}>{},
      Anim>;

  constexpr auto proto =
      spec::get_protobuilder()
          .transition(a, b,
                      [](const machine::execute::state &state,
                         Anim &data) -> machine::execute::transition {
                        for (; not state.is_interrupted();) {
                          // do job
                          co_yield machine::execute::result::process;
                        }

                        if (state.is_interrupted()) {
                          co_yield machine::execute::result::success_interrupt;
                        }

                        co_return;
                      })
          .stay(a,
                [](const machine::execute::state &state,
                   Anim &) -> machine::execute::stay {
                  for (; not state.is_interrupted();) {
                    // do job
                    co_yield machine::execute::result::process;
                  }

                  if (state.is_interrupted()) {
                    co_yield machine::execute::result::success_interrupt;
                  }

                  co_return base::idle;
                })
          .finalize();

  auto machine_ptr = spec::make_instance(proto);

  machine_ptr->process();
};
