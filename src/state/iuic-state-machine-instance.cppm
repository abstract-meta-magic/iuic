// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:machine.instance;
import :machine.execute;
import :machine.controller;

export namespace iuic::state::machine {
struct instance {
  virtual ~instance() = default;

  virtual controller &get_controller() = 0;

  virtual const execute::state &get_execute_state() const = 0;

  virtual std::size_t get_spec_id() const = 0;

  virtual void process() = 0;
};
}; // namespace iuic::state::machine
