// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.text;
import :atlas;

namespace iuic::text {
struct atlas_registry_handler__ {
  const atlas *ptr;
  atlas::id id;
};
}; // namespace iuic::text

namespace {

auto &registry_unsafe__() {
  using namespace iuic::text;

  static atlas invalid{};

  struct registry {
    registry() {
      for (atlas::id cur{2500}; cur < 7500; ++cur) {
        id_pool__.push(cur);
      }
    };

    atlas_registry_handler__ *attach(const atlas &atlas) {
      const iuic::text::atlas *ptr = std::addressof(atlas);
      if (id_pool__.empty()) {
        throw "error";
      }

      if (name_store__.contains(atlas.name)) {
        throw "error";
      }

      atlas::id id = id_pool__.front();
      id_pool__.pop();

      ptr_store__.insert({id, ptr});
      id_store__.insert({ptr, id});
      name_store__.insert({atlas.name, ptr});
      return new atlas_registry_handler__{.ptr = ptr, .id = id};
    };

    void detach(atlas_registry_handler__ *handler) {
      if (auto id = id_store__.find(handler->ptr); id != id_store__.end()) {
        ptr_store__.erase(id->second);
        name_store__.erase(handler->ptr->name);
        id_store__.erase(id->first);
        id_pool__.push(id->second);
      }

      delete handler;
    };

    const atlas &by_name(std::string_view name) {
      if (auto atlas = name_store__.find(std::string{name});
          atlas != name_store__.end()) {
        return *atlas->second;
      } else {
        return invalid;
      }
    };

    const atlas &by_symname(std::string_view name) { return invalid; };
    const atlas &by_id(atlas::id id) {
      if (auto atlas = ptr_store__.find(id); atlas != ptr_store__.end()) {
        return *atlas->second;
      } else {
        return invalid;
      }
    };
    atlas::id get_id(const atlas &atlas) {
      if (auto id = id_store__.find(std::addressof(atlas));
          id != id_store__.end()) {
        return id->second;
      } else {
        return atlas::invalid_id;
      };
    };

    std::unordered_map<atlas::id, const atlas *> ptr_store__;
    std::unordered_map<const atlas *, atlas::id> id_store__;
    std::unordered_map<std::string, const atlas *> name_store__;
    std::queue<atlas::id> id_pool__;
  } static registry_instance;

  return registry_instance;
};

auto registry_safe__() {
  static std::mutex registry_mtx{};
  using registry_ref = decltype(registry_unsafe__());
  struct proxy {
    registry_ref instance;
    proxy(registry_ref &instance_) : instance{instance_} {};
    proxy(const proxy &) = delete;
    proxy &operator=(const proxy &) = delete;
    proxy(proxy &&) = delete;
    proxy &operator=(proxy &&) = delete;

  private:
    std::unique_lock<std::mutex> lock__{registry_mtx};
  };
  return proxy{registry_unsafe__()};
};
}; // namespace

namespace iuic::text {
atlas_registry_handler__ *atlas::registry_attach(atlas &atlas) {
  return registry_safe__().instance.attach(atlas);
};
void atlas::registry_detach(atlas_registry_handler__ *heandler) {
  registry_safe__().instance.detach(heandler);
};

atlas::id atlas::get_id() const {
  return registry_safe__().instance.get_id(*this);
};

atlas::id atlas::get_id(const atlas &atlas) {
  return registry_safe__().instance.get_id(atlas);
};

const atlas &atlas::by_name(std::string_view name) {
  return registry_safe__().instance.by_name(name);
};
const atlas &atlas::by_symname(std::string_view name) {
  return registry_safe__().instance.by_symname(name);
};
const atlas &atlas::by_id(id id) {
  return registry_safe__().instance.by_id(id);
};
}; // namespace iuic::text
