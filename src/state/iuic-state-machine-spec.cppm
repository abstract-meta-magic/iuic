// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:machine.spec;
import :machine.graph;
import :machine.execute;
import :machine.instance;

export namespace iuic::state::machine {

template <typename T>
concept is_spec = requires(T &obj, typename T::prototype &proto) {
  obj.get_instance(proto);
  T::get_protobuilder();
  T::runtime_id();
};

template <typename T>
concept is_prototype = requires(T &obj, state::value value) {
  typename T::spec;
  typename T::stay_ctor_t;
  typename T::transition_ctor_t;
  requires is_spec<typename T::spec>;
  //
  obj.get_stay_ctor(value)->T::stay_ctor_t;
  obj.get_transition_ctor(value, value)->T::transition_ctor_t;
  obj.get_entry()->state::value;
  obj.get_exception_handler()->T::stay_ctor_t;
  obj.get_terminate_handler()->T::transition_ctor_t;
};

template <transition_graph graph, iuic::erasure::is_pure_type SharedData>
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

    constexpr prototype_base(value entry_, stay_ctor_t excp_ctor,
                             transition_ctor_t term_ctor,
                             transition_ctor_map_t tr_map,
                             stay_ctor_map_t st_map) noexcept
        : entry{entry_}, exception_handler_ctor{excp_ctor},
          terminate_handler_ctor{}, transition_ctor_map{tr_map},
          stay_ctor_map{st_map} {};

  protected:
    value entry;
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

    value get_entry() const { return prototype_base::entry; };

    prototype_base::stay_ctor_t get_exception_handler() const {
      return prototype_base::exception_handler_ctor;
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
      prototype_base::entry = val;
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

  static std::size_t runtime_id() {
    static void *_{};
    return reinterpret_cast<std::size_t>(_);
  };

  struct instance final : public machine::instance, private controller {
  private: // controller
    bool can_move(state::value st) const override {
      return graph.transition_index(state.active, st) != graph.invalid_index;
    };

    // выставляет следующее состояние
    bool try_move(state::value nstate) override {
      if (can_move(nstate)) {
        selected = nstate;
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
      st_handler = p.get_stay_ctor(p.get_entry())(state, data);
      state.value = execute::state::stay;
      state.active = p.get_entry();
    };

    controller &get_controller() override { return *this; };

    const execute::state &get_execute_state() const override { return state; };

    std::size_t get_spec_id() const override { return spec::runtime_id(); };

    void process() override {
      if (state.active == base::exception_handling) {
        exception_handling();
      } else {
        selected == state::base::null ? common_marshalling__()
                                      : interruptr_marshalling__();
      }
    };

  private: // interruptr
    void exception_handling() {
      st_handler.process();

      if (st_handler.last_yield() ==
          execute::result::terminate_by_unexpected_exception) {
        // TODO : TERMINATE MACHINE
      }

      if (st_handler.finished()) {
        auto nstate = st_handler.return_value();

        state.reverse();
        if (graph.stay_index(nstate) == graph.invalid_index) {
          std::println("inv");
          nstate = prototype.get_entry();
        }

        state.active = nstate;
        st_handler = prototype.get_stay_ctor(nstate)(state, data);
        state.value = state.stay;
      }
    };

    void set_exception(const std::exception_ptr &excpthion) {
      st_handler = prototype.get_exception_handler()(state, data);
      tr_handler = execute::transition{nullptr};
      state.exception = excpthion;
      state.reverse();
      state.active = base::exception_handling;
      state.value = state.stay;

      exception_handling(); // <-- run
    };

    template <typename T> void interruptr_marshalling__final__(T &handler) {
      if (handler.last_yield() == execute::result::success_interrupt) {
        tr_handler =
            prototype.get_transition_ctor(state.active, selected)(state, data);
        state.reverse();
        state.to = std::exchange(selected, base::null);
        state.value = state.transition;
      }

      if (handler.last_yield() != execute::result::process_interrupt) {
        // TODO : error check
      }
    };

    template <execute::state::value_t st, typename T>
    void interruptr_marshalling__off_process__(T &handler) {
      state.value = st;
      handler.process();

      if (handler.exception()) {
        set_exception(handler.exception());
      } else {
        interruptr_marshalling__final__(handler);
      }
    };

    template <typename T>
    void interruptr_marshalling__in_process__(T &handler) {
      if (handler.last_yield() == execute::result::process_interrupt) {
        handler.process();
        if (handler.exception()) {
          set_exception(handler.exception());
          return;
        }
      }

      interruptr_marshalling__final__(handler);
    };

    void interruptr_marshalling__() {
      switch (state.value) {
      case execute::state::stay: {
        interruptr_marshalling__off_process__<execute::state::stay_interrupt>(
            st_handler);
        break;
      }
      case execute::state::stay_interrupt: {
        interruptr_marshalling__in_process__(st_handler);
        break;
      }
      case execute::state::transition: {
        interruptr_marshalling__off_process__<
            execute::state::transition_interrupt>(tr_handler);
        break;
      }
      case execute::state::transition_interrupt: {
        interruptr_marshalling__in_process__(tr_handler);
        break;
      }
      default: {
        // error check
        break;
      }
      }
    };

  private: // common
    void common_marshalling__() {
      switch (state.value) {
      case execute::state::stay: {
        st_handler.process();
        if (st_handler.exception()) {
          set_exception(st_handler.exception());
          return;
        }

        auto nstate = st_handler.return_value();
        if (st_handler.finished()) {
          if (graph.transition_index(state.active, nstate) !=
              graph.invalid_index) {
            tr_handler = prototype.get_transition_ctor(state.active,
                                                       nstate)(state, data);

            state.value = state.transition;
            state.reverse();
            state.to = nstate;
          } else {
            set_exception(std::make_exception_ptr(impossible_transition{}));
          }
        }
        break;
      }
      case execute::state::transition: {
        tr_handler.process();
        if (tr_handler.finished()) {
          st_handler = prototype.get_stay_ctor(state.to)(state, data);
          state.value = state.stay;
        }
        break;
      }
      default: {
        // throw exception
      };
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

  static constexpr std::unique_ptr<machine::instance>
  make_instance(const prototype &proto) {
    return std::make_unique<instance>(proto);
  };
};
}; // namespace iuic::state::machine
