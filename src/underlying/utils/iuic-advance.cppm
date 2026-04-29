// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors
export module iuic.underlying.utils:advance;
import std;

// Thread unsafe!!!
// Mb use mutex
export namespace iuic::advance {
struct interface;

struct pool final {
  void advance();

  static pool &global();

  ~pool();

private:
  friend interface;
  void attach(interface *ptr);

  void detach(interface *ptr);

  void remove(interface *ptr);

  std::unordered_set<interface *> objs;
};

struct interface {

  void rebind(pool *npool) {
    if (not npool || npool == pool)
      return;

    before_rebind();

    if (pool)
      pool->detach(this);

    npool->attach(this);
    std::swap(npool, pool);
    after_rebind();
  };

  void rebind(pool &npool) { rebind(std::addressof(npool)); }

  void rebind(interface *other) { rebind(other->pool); };

  void rebind(interface &other) { rebind(std::addressof(other)); };

  interface(pool *npool) : pool{npool} { pool->attach(this); }

  interface(pool &npool) : interface{std::addressof(npool)} {}

  interface() : interface{pool::global()} {}

protected:
  virtual void advance() {};

  virtual void before_rebind() {};

  virtual void after_rebind() {};

  ~interface() {
    if (pool) {
      pool->detach(this);
    }
  };

private:
  pool *pool;
  friend void pool::remove(interface *);
  friend void pool::advance();
};

}; // namespace iuic::advance
