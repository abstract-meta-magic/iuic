
module;

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

  static constexpr state null() { return nullptr; };

  const decl *value{nullptr};
};

struct state::decl final {
  consteval decl() noexcept {};

  decl(const decl &) = delete;
  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  constexpr operator const decl *() const noexcept { return this; }

  // force static\global
  const decl *self{this};
};

struct state::base {
  static constexpr state hovered() {
    static constexpr state::decl _;
    return _;
  };
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

    static constexpr std::size_t state_count = []() { return 0; }();
  };

  struct control_block {
    enum class state {
      transit,
      execute,
      interrupt,
    };

    virtual ~control_block() = default;

    virtual constexpr bool can_move(iuic::state) const = 0;

    virtual constexpr bool try_move(iuic::state) = 0;

    virtual constexpr bool force_move(iuic::state) = 0;

    virtual constexpr state get_state() const = 0;

    virtual constexpr void rethrow() const = 0;

    // visit shared
    template <typename T>
    constexpr bool try_visit_shared(
        erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call)
        const {
      get_shared_object().try_visit(std::forward<decltype(call)>(call));
    };

  protected:
    virtual erasure::visited::as_mutable get_shared_object() const = 0;
  };

  struct machine_block : control_block {
    virtual ~machine_block() = default;

    virtual constexpr void process() = 0;

    virtual constexpr std::size_t spec_id() const = 0;
  };

  template <iuic::state, iuic::state, auto process>
  struct transition_process {};
  template <iuic::state, auto process> struct state_process {};

  template <transition_graph gpaph, iuic::erasure::as_pure_type SharedData>
  struct spec {
    using handler_type = int;
    using shared_type = SharedData;

    struct prototype {
      using spec = spec;

      // tuple transition_process

      // tuple state_process
    };

    struct machine_block : machine::machine_block {
      machine_block(const prototype &);

      constexpr bool can_move(iuic::state) const override{};

      constexpr bool try_move(iuic::state) override{};

      constexpr bool force_move(iuic::state) override{};

      constexpr void process() override {};

      constexpr std::size_t spec_id() const override {};

      constexpr state get_state() const override { iuic::state::null(); };

      constexpr void rethrow() const override {};

      erasure::visited::as_mutable get_shared_object() const {
        return erasure::visited::as_mutable{nullptr};
      };
    };

    static constexpr prototype make_prototype(auto... state_handler) {
      return {};
    };

    static constexpr machine_block make_machine_block(const prototype &proto) {
      return machine_block{proto};
    };
  };
};
}; // namespace iuic

namespace std {
export template <> struct std::hash<iuic::state> {
  std::size_t operator()(const iuic::state &s) const noexcept { return s; }
};

}; // namespace std
