// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors
module iuic.core;
import iuic.underlying;

import :state;
using namespace iuic::test;

template ticket usecase<unit<[](utils &test) {
  using namespace iuic::state;
  static constexpr decl a = decl::instance_of<a>();
  static constexpr decl b = decl::instance_of<b>();

  struct Data {
    int counter{0};
    int a{0};
    int b{0};
    int ab{0};
    int ba{0};
    int i{0};
  };
  auto spec = machine::spec<
      machine::transition_graph<machine::transition{a, b, true}>{}, Data>{};

  auto proto =
      spec.get_protobuilder()
          .entry<b>()
          .transition(a, b,
                      [](const machine::execute::state &state,
                         Data &data) -> machine::execute::transition {
                        int i{0};
                        for (; not state.is_interrupted() && i++ < 2;) {
                          std::println("tr : a -> b");
                          ++data.ab;
                          co_yield machine::execute::result::process;
                        }

                        if (state.is_interrupted()) {
                          ++data.i;
                          co_yield machine::execute::result::success_interrupt;
                        }
                        co_return;
                      })
          .transition(b, a,
                      [](const machine::execute::state &state,
                         Data &data) -> machine::execute::transition {
                        int i{0};
                        for (; not state.is_interrupted() && i++ < 4;) {
                          std::println("tr : b -> a");
                          ++data.ba;
                          co_yield machine::execute::result::process;
                        }

                        if (state.is_interrupted()) {
                          ++data.i;
                          co_yield machine::execute::result::success_interrupt;
                        }

                        co_return;
                      })
          .stay(b,
                [](const machine::execute::state &state,
                   Data &data) -> machine::execute::stay {
                  for (; not state.is_interrupted();) {
                    std::println("stay : b");
                    ++data.b;
                    co_yield machine::execute::result::process;
                  }

                  if (state.is_interrupted()) {
                    ++data.i;
                    co_yield machine::execute::result::success_interrupt;
                  }

                  co_return base::idle;
                })
          .stay(a,
                [](const machine::execute::state &state,
                   Data &data) -> machine::execute::stay {
                  for (; not state.is_interrupted();) {
                    std::println("stay : a");
                    ++data.a;
                    co_yield machine::execute::result::process;
                  }

                  if (state.is_interrupted()) {
                    ++data.i;
                    co_yield machine::execute::result::success_interrupt;
                  }

                  co_return base::idle;
                })
          .finalize();

  auto machine = spec.make_instance(proto);
  auto &c = machine->get_controller();

  test.eq(c.try_visit_shared([&](Data &data) { test.eq(data.b, 0); }))
      .assertion_true("visit data");

  machine->process();

  test.eq(c.try_visit_shared([&](Data &data) { test.neq(data.b, 0); }))
      .assertion_true("visit data");

  machine->get_controller().try_move(a);

  machine->process();

  test.eq(c.try_visit_shared([&](Data &data) {
        test.eq(data.i, 1).assertion_true("success interrupt");
      }))
      .assertion_true("visit data");

  machine->process();
  machine->process();
  machine->process();
  machine->process();
  machine->process();
  test.eq(c.try_visit_shared([&](Data &data) {
        test.eq(data.ba, 4).assertion_true("success job");
      }))
      .assertion_true("visit data");

  machine->process();
  machine->process();

  test.eq(c.try_visit_shared([&](Data &data) {
        test.eq(data.a, 2).assertion_true("success job");
      }))
      .assertion_true("visit data");
}>{}>;

template ticket usecase<unit<[](utils &test) {
  using namespace iuic::state;
  static constexpr decl a = decl::instance_of<a>();
  static constexpr decl b = decl::instance_of<b>();

  struct Data {
    int value{0};
  };

  auto spec = machine::spec<
      machine::transition_graph<machine::transition{a, b, true}>{}, Data>{};

  auto proto = spec.get_protobuilder()
                   .entry<a>()
                   .stay(a,
                         [](const machine::execute::state &state,
                            Data &data) -> machine::execute::stay {
                           for (; not state.is_interrupted();) {
                             ++data.value;
                             co_yield machine::execute::result::process;
                           }
                           co_return base::null;
                         })
                   .finalize();

  constexpr iuic::units::uid au{44};
  constexpr iuic::units::uid bu{48};

  iuic::advance::pool p;

  machine::dispatcher dp;
  dp.rebind(p);

  dp.machine_instance(au, proto);
  dp.machine_instance(bu, proto);

  test.eq(dp.get_machine(au)).assertion_true("Machine [a] exist");

  std::println("run ----- before advance");
  dp.execute();

  auto *machine_1 = dp.get_machine(au);
  auto *machine_2 = dp.get_machine(bu);

  if (test.eq(machine_1)
          .assertion_true("get-machine [a]")
          .assertion_false("machine-nonexist [a]")) {
    machine_1->get_controller().try_visit_shared([&](Data &data) {
      test.eq(data.value, 1).assertion_true("correct process [a]");
    });
  }
  if (test.eq(machine_2)
          .assertion_true("get-machine [b]")
          .assertion_false("machine-nonexist [b]")) {
    machine_2->get_controller().try_visit_shared([&](Data &data) {
      test.eq(data.value, 1).assertion_true("correct process [a]");
    });
  }

  p.advance();
  std::println("run ----- after advance");
  test.neq(dp.get_machine(au)).assertion_true("machine-advanced [a]");
  test.neq(dp.get_machine(bu)).assertion_true("machine-advanced [b]");
  dp.execute();
  if (test.eq(machine_1)
          .assertion_true("get-machine [a]")
          .assertion_false("machine-nonexist [a]")) {
    machine_1->get_controller().try_visit_shared([&](Data &data) {
      test.eq(data.value, 1).assertion_true("correct process [a]");
    });
  }
  if (test.eq(machine_2)
          .assertion_true("get-machine [b]")
          .assertion_false("machine-nonexist [b]")) {
    machine_2->get_controller().try_visit_shared([&](Data &data) {
      test.eq(data.value, 1).assertion_true("correct process [a]");
    });
  }

  std::println("update machine -----");
  dp.machine_instance(au, proto);
  dp.machine_instance(bu, proto);
  std::println("run ----- after update");
  dp.execute();
  if (test.eq(machine_1)
          .assertion_true("get-machine [a]")
          .assertion_false("machine-nonexist [a]")) {
    machine_1->get_controller().try_visit_shared([&](Data &data) {
      test.eq(data.value, 2).assertion_true("correct process [a]");
    });
  }
  if (test.eq(machine_2)
          .assertion_true("get-machine [b]")
          .assertion_false("machine-nonexist [b]")) {
    machine_2->get_controller().try_visit_shared([&](Data &data) {
      test.eq(data.value, 2).assertion_true("correct process [a]");
    });
  }
}>{}>;
