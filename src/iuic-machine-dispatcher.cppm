// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.core:machine.dispatcher;
import std;
import iuic.underlying;
import iuic.state;

namespace iuic::state::machine {

struct dispatcher : public advance::interface {
  void execute() {

    auto &buf = pool.get_buffers().current;

    std::for_each(buf.begin(), buf.end(), [](auto &&machine) {
      if (machine.second)
        machine.second->process();
    });
  };

  instance *get_machine(iuic::units::uid uid) {
    return pool.get_buffers().current.contains(uid)
               ? pool.get_buffers().current.at(uid).get()
               : nullptr;
  };

  const instance *get_machine(iuic::units::uid uid) const {
    return pool.get_buffers().current.contains(uid)
               ? pool.get_buffers().current.at(uid).get()
               : nullptr;
  };

  // init or update livetime
  bool machine_instance(iuic::units::uid uid, const auto &prototype) {
    using spec = typename std::remove_cvref_t<decltype(prototype)>::spec;

    auto bufs = pool.get_buffers();

    if (not bufs.current.contains(uid)) {
      if (bufs.prev.contains(uid)) {
        pool.move_forward(uid);
      } else {
        auto instance = spec::make_instance(prototype);
        bufs.current.insert(std::pair{uid, std::move(instance)});
      }
    }

    if (bufs.current.at(uid)->get_spec_id() != spec::id) {
      auto instance = spec::make_instance(prototype);
      bufs.current.at(uid).swap(instance);
    }

    return true;
  };

private: // advance
  void advance() override { pool.swap(); };

private:
  using pool_t = utils::swap_buffers<
      std::unordered_map<iuic::units::uid, std::unique_ptr<instance>>, 2>;

  pool_t pool;
};
}; // namespace iuic::state::machine
