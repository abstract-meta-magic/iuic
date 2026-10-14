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

namespace query {

template <typename T> struct type {};

struct basic_pred {};

export template <typename T>
concept is_pred = std::is_base_of_v<basic_pred, std::remove_cvref_t<T>>;

export constexpr type<asset::definition> definition;

export constexpr type<asset::instance> instance;

export template <typename T> struct has_bind_t : basic_pred {};

export template <typename T> constexpr has_bind_t<T> has_bind;

export struct has_tag : basic_pred {
  has_tag(asset::tag tag_) : tag{tag_} {};
  asset::tag tag;
};

export struct as_type : basic_pred {
  as_type(asset::type type_) : type{type_} {};
  asset::type type;
};

export struct name_contains : basic_pred {
  name_contains(std::string_view str_) : str{str_} {};
  std::string_view str;
};

export template <is_pred T> struct qnot : basic_pred {
  qnot(T pred_) : pred{pred_} {};
  T pred;
};
}; // namespace query

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
    if (a.has_value()) {
      for (auto &inst : a.get().instances) {
        remove_instance(inst);
      };
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

  bool contains(instance_id iid) {
    return records.instances.has_value(std::to_underlying(iid));
  };

  bool try_visit_definition(
      declaration decl,
      std::invocable<const definition &> auto &&call) const noexcept {
    auto a = records.definitions.access(decl);
    if (a.has_value()) {
      call(a.get().def);
      return true;
    };
    return false;
  };

  bool try_visit_definition(declaration decl,
                            std::invocable<definition &> auto &&call) {
    auto a = records.definitions.access(decl);
    if (a.has_value()) {
      call(a.get().def);
      return true;
    };
    return false;
  };

  // used before defined ?
  [[nodiscard("Instance leak")]] instance_id
  make_dynamic_instance(declaration decl) {
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
  [[nodiscard("Instance leak")]] instance_id
  make_dynamic_instance(declaration decl, Binds &&...binds) {
    auto iid = make_dynamic_instance(decl);

    if (iid != invalid_id) {
      auto &inst = get_instance(iid);

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
      auto &inst = get_instance(iid);

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
      auto &inst = get_instance(iid);

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

  // TODO
  // in std-c++ 26
  // std::optional<instance&> try_get_instance(instance_id id_);

  instance &get_instance(instance_id id_) {
    if (records.instances.has_value(std::to_underlying(id_))) {
      return records.instances.at(std::to_underlying(id_)).inst;
    }
    throw std::logic_error{"Invalid instance ID"};
  };

  const instance &get_instance(instance_id id_) const {
    if (records.instances.has_value(std::to_underlying(id_))) {
      return records.instances.at(std::to_underlying(id_)).inst;
    }
    throw std::logic_error{"Invalid instance ID"};
  };

  bool try_visit_instance(instance_id id_,
                          std::invocable<instance &> auto &&call) noexcept {
    if (records.instances.has_value(std::to_underlying(id_))) {
      call(records.instances.at(std::to_underlying(id_)).inst);
      return true;
    }
    return false;
  };

  bool try_visit_instance(
      instance_id id_,
      std::invocable<const instance &> auto &&call) const noexcept {
    if (records.instances.has_value(std::to_underlying(id_))) {
      call(records.instances.at(std::to_underlying(id_)).inst);
      return true;
    }
    return false;
  };

  void reset();

  bool is_static(instance_id iid) const {
    auto index = std::to_underlying(iid);
    if (records.instances.has_value(index)) {
      return records.instances.at(index).type == instance_record::Static;
    }
    return false;
  };
  bool is_dynamic(instance_id iid) const {
    auto index = std::to_underlying(iid);
    if (records.instances.has_value(index)) {
      return records.instances.at(index).type == instance_record::Dynamic;
    }
    return false;
  };

  bool is_default(instance_id iid) const {
    auto index = std::to_underlying(iid);
    if (records.instances.has_value(index)) {
      return records.instances.at(index).type == instance_record::Default;
    }
    return false;
  };

public: // QUERY
  std::vector<instance_id> query(query::type<instance>,
                                 query::is_pred auto &&...preds) const {
    std::vector<instance_id> res;
    for (auto &ir : records.instances) {
      if ((do_eval(ir, preds) && ...)) {
        // make distance ???
        utils::assert([] { return false; }, "TODO: support base query");
      };
    }
    return res;
  };

  void query(query::type<instance>, func::as<void(instance &)> auto &&func,
             query::is_pred auto &&...preds) {
    for (auto &ir : records.instances) {
      if ((do_eval(ir, preds) && ...)) {
        std::invoke(std::forward<decltype(func)>(func), ir.inst);
      };
    }
  };

  void query(query::type<instance>,
             func::as<void(const instance &)> auto &&func,
             query::is_pred auto &&...preds) const {
    for (auto &ir : records.instances) {
      if ((do_eval(ir, preds) && ...)) {
        std::invoke(std::forward<decltype(func)>(func), ir.inst);
      };
    }
  };

  std::vector<declaration> query(query::type<definition>,
                                 query::is_pred auto &&...preds) const {
    std::vector<declaration> res;
    for (auto &dr : records.definitions) {
      if ((do_eval(dr, preds) && ...)) {
        res.push_back(dr.def.of());
      }
    }
    return res;
  };

  void query(query::type<definition>, func::as<void(definition &)> auto &&func,
             query::is_pred auto &&...preds) {
    for (auto &dr : records.definitions) {
      if ((do_eval(dr, preds) && ...)) {
        std::invoke(std::forward<decltype(func)>(func), dr.def);
      }
    }
  };

  void query(query::type<definition>,
             func::as<void(const definition &)> auto &&func,
             query::is_pred auto &&...preds) const {
    for (auto &dr : records.definitions) {
      if ((do_eval(dr, preds) && ...)) {
        std::invoke(std::forward<decltype(func)>(func), dr.def);
      }
    }
  };

private:
  bool do_eval(const definition_record &dr,
               const query::name_contains &pred) const {
    return dr.def.of().get_name().contains(pred.str);
  };

  bool do_eval(const definition_record &dr, const query::as_type &pred) const {
    return dr.def.get_type() == pred.type;
  };

  bool do_eval(const definition_record &dr, const query::has_tag &pred) const {
    return dr.def.has_tag(pred.tag);
  };

  template <typename T>
  bool do_eval(const definition_record &dr,
               const query::has_bind_t<T> &pred) const {
    return dr.def.get_bind<T>().has_value();
  };

  bool do_eval(const instance_record &ir,
               const query::name_contains &pred) const {
    return ir.inst.of().of().get_name().contains(pred.str);
  };

  bool do_eval(const instance_record &ir, const query::as_type &pred) const {
    return ir.inst.get_type() == pred.type;
  };

  bool do_eval(const instance_record &ir, const query::has_tag &pred) const {
    return ir.inst.has_tag(pred.tag);
  };

  template <typename T>
  bool do_eval(const instance_record &ir,
               const query::has_bind_t<T> &pred) const {
    return ir.inst.get_bind<T>().has_value();
  };

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
      records.instances.erase(id);
    };
    // remove
    // remove in def list
  };

private:
  catalog_records records;
};
}; // namespace iuic::asset
