// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.env:persist;
import std;
import iuic.underlying;
import iuic.state;
import iuic.style;
export import :persist.object;
export import :persist.state;

export namespace iuic::environment {

struct persist {

  persist_object_storage object;

  persist_state_storage state;

  // and more
  persist(advance::pool &pool) {
    object.rebind(pool);
    state.rebind(pool);
  };

public: // BIG-V
  persist(const persist &) = delete;
  persist(persist &&) = delete;
  persist &operator=(const persist &) = delete;
  persist &operator=(persist &&) = delete;
  ~persist() {};
};

}; // namespace iuic::environment
