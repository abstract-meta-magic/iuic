// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:catalog;
import std;
import :base;
import :declaration;
import :static_instance;
import :definition;
import :map;
import :free_list;
import :instance;
import :bind;

namespace iuic::asset {

struct definition_record {
  definition def;
  instance_id default_instance{invalid_id};
  std::vector<instance_id> instances;
};

struct instance_record {
  instance inst;
  std::variant<declaration, static_instance> of;
  enum { Static, Dynamic, Default } type{Static};
};

namespace map {
template <> struct hash_from<declaration> {
  static hash_t operator()(const declaration &decl) { return decl.get_hash(); };
};

template <> struct hash_from<static_instance> {
  static hash_t operator()(const static_instance &ins) {
    return ins.get_hash();
  };
};

}; // namespace map

// TODO: work with error's
struct catalog_records {
  container<map::fixed<declaration, definition_record, 1024>> definitions;
  container<map::fixed<static_instance, instance_id, 512>>
      definitions_static_instances;
  container<free_list::fixed<instance_record, 4096>> instances;
};

struct definition_query_proxy {
  constexpr definition_query_proxy(const catalog_records &cr) : records{cr} {};

private:
  const catalog_records &records;
};

struct instance_query_proxy {
  constexpr instance_query_proxy(const catalog_records &cr) : records{cr} {};

private:
  const catalog_records &records;
};

export struct catalog {
  // rcode and std::expected for noexcept API
  enum class rcode { Success, UnexpectedError, UsedBeforeDefined, Overflow };

  void make_definition(declaration decl) {
    auto a = records.definitions.access(decl);
    if (not a.has_value()) {
      std::move(a).set(definition_record{definition{decl}, invalid_id, {}});
    }
  };

  void remove_definition(declaration decl) {
    auto a = records.definitions.access(decl);
    if (not a.has_value()) {
      std::move(a).reset();
    }
  };

  definition &get_definition(declaration decl) {
    auto a = records.definitions.access(decl);
    if (a.has_value()) {
      return a.get().def;
    };
    throw std::logic_error{"is not defined"};
  };

  bool contains(declaration decl) {
    return records.definitions.access(decl).has_value();
  };

  // used before defined ?
  instance_id make_dynamic_instance(declaration decl) {
    if (auto access = records.definitions.access(decl); access.has_value()) {
      auto &record = access.get();
      auto iid = instance_id{records.instances.insert(instance_record{
          instance{&record.def}, decl, instance_record::Dynamic})};
      if (iid != invalid_id) {
        return iid;
      }
    };
    return invalid_id;
  };

  template <typename... Binds>
  instance_id make_dynamic_instance(declaration decl, Binds &&...binds) {
    auto iid = make_default_instance(decl);

    if (iid != invalid_id) {
      auto &inst = get_instance(iid).value().get();

      (inst.get_bind<std::remove_cvref_t<Binds>>().emplace(
           std::forward<Binds>(binds)),
       ...);
    };
    return iid;
  };

  instance_id make_default_instance(declaration decl) {
    if (auto access = records.definitions.access(decl); access.has_value()) {
      auto &record = access.get();
      if (record.default_instance == invalid_id) {
        // can return invalid_id, but this ok
        record.default_instance =
            instance_id{records.instances.insert(instance_record{
                instance{&record.def}, decl, instance_record::Default})};
      }
      return record.default_instance;
    }
    return invalid_id;
  };

  template <typename... Binds>
  instance_id make_default_instance(declaration decl, Binds &&...binds) {
    auto iid = make_default_instance(decl);

    if (iid != invalid_id) {
      auto &inst = get_instance(iid).value().get();

      (inst.get_bind<std::remove_cvref_t<Binds>>().emplace(
           std::forward<Binds>(binds)),
       ...);
    };
    return iid;
  };

  instance_id get_default_instance(declaration decl) {
    if (auto access = records.definitions.access(decl); access.has_value()) {
      return access.get().default_instance;
    }
    return invalid_id;
  };

  // requires : decl before instance
  instance_id make_static_instance(static_instance si) {
    if (auto def_found = records.definitions.access(si.of());
        def_found.has_value()) {
      if (auto ins_found = records.definitions_static_instances.access(si);
          ins_found.has_value()) {
        return ins_found.get();
      } else {
        auto &def_record = def_found.get();
        auto insert = instance_id{records.instances.insert(instance_record{
            instance{&def_record.def}, si, instance_record::Static})};

        if (insert != invalid_id) {
          def_record.instances.push_back(insert);
          std::move(ins_found).set(insert);
          return insert;
        }
      }
    }

    return invalid_id;
  };

  template <typename... Binds>
  instance_id make_static_instance(static_instance si, Binds &&...binds) {
    auto iid = make_static_instance(si);

    if (iid != invalid_id) {
      auto &inst = get_instance(iid).value().get();

      (inst.get_bind<std::remove_cvref_t<Binds>>().emplace(
           std::forward<Binds>(binds)),
       ...);
    };
    return iid;
  };

  instance_id get_static_instance(static_instance si) {
    if (auto found = records.definitions_static_instances.access(si);
        found.has_value()) {
      return found.get();
    };
    return invalid_id;
  };

  // static/dynamic/default
  void remove_instance(instance_id id_) {
    std::size_t I{std::to_underlying(id_)};
    if (records.instances.has_value(I)) {
      auto &instance_record = records.instances.at(I);
      switch (instance_record.type) {
      case instance_record.Static: {
        remove_static_instance_(id_, instance_record);
        break;
      }
      case instance_record.Dynamic: {
        remove_dynamic_instance_(id_, instance_record);
        break;
      }
      case instance_record.Default: {
        remove_default_instance_(id_, instance_record);
        break;
      }
      };
    }
  };

  std::expected<std::reference_wrapper<instance>, int>
  get_instance(instance_id id_) {

    utils::assert([&] { return id_ != invalid_id; },
                  "try access to invalid instance");

    std::println("index {}", std::to_underlying(id_));
    if (records.instances.has_value(std::to_underlying(id_))) {
      return records.instances.at(std::to_underlying(id_)).inst;
    }
    return std::unexpected{0};
  };
  // call on_reset(...) callback's
  void reset();

  // query_result is std::vector<instance_id>
  instance_query_proxy instance_query();
  // query_result is std::vector<declaration>
  definition_query_proxy definition_query();

private:
  void remove_dynamic_instance_(instance_id id, instance_record &ir) {
    auto &decl = std::get<declaration>(ir.of);
    auto &dr = records.definitions.access(decl).get();
    if (auto found = std::find(dr.instances.begin(), dr.instances.end(), id);
        found != dr.instances.end()) {
      std::swap(*found, dr.instances.back());
      dr.instances.pop_back();
      records.instances.erase(std::to_underlying(id));
    }
  };
  void remove_default_instance_(instance_id id, instance_record &ir) {
    auto &decl = std::get<declaration>(ir.of);
    auto &dr = records.definitions.access(decl).get();
    if (dr.default_instance == id) {
      dr.default_instance = invalid_id;
      records.instances.erase(std::to_underlying(id));
    }
  };

  void remove_static_instance_(instance_id id_, instance_record &ir) {
    auto id = std::to_underlying(id_);
    auto inst = std::get<static_instance>(ir.of);
    auto &def = records.definitions.access(inst.of()).get();

    if (auto found = std::find(def.instances.begin(), def.instances.end(), id_);
        found != def.instances.end()) {
      std::swap(*found, def.instances.back());
      def.instances.pop_back();
      records.definitions_static_instances.access(inst).reset();
    };
    // remove
    // remove in def list
  };

private:
  catalog_records records;
};
}; // namespace iuic::asset
