

module;

export module iuic.core:storage.text;
import :hash;
import :storage.def;
import :text.buff;

namespace iuic {

// Класс работает с распределением памяти для текста
// предоставляя аллокаторы для text_buffer и другое.
struct text_storage {
  // или может сделать как в хранилище
  // через специальное id ???

  bool try_visit(trk_t trk, std::invocable<text::buffer &> auto &&call) {
    if (stage == __1) {
      if (stage_1__.contains(trk)) {
        call(stage_1__.at(trk));
      }
    } else {
      if (stage_2__.contains(trk)) {
        call(stage_2__.at(trk));
      }
    };
  };

  bool try_visit(trk_t trk,
                 std::invocable<const text::buffer &> auto &&call) const {
    if (stage == __1) {
      if (stage_1__.contains(trk)) {
        call(stage_1__.at(trk));
      }
    } else {
      if (stage_2__.contains(trk)) {
        call(stage_2__.at(trk));
      }
    };
  };

  // trk | srk
  // text_registry_key | storage_registry_key
protected: // trk
  static trk_t make_trk__(uid_t uid, const std::string &str) noexcept {
    static std::int64_t seed{224457};

    auto res = str + std::to_string(uid);

    return 0;
  };

protected:
  enum {
    __1,
    __2,
  } stage{__1};
  std::unordered_map<trk_t, text::buffer> stage_1__;
  std::unordered_map<trk_t, text::buffer> stage_2__;
};

// WRONG
struct tkk : text::tokenizer {
  text::token::sequence process(std::string_view str) const noexcept override {
    return {};
  };
};

struct mutable_text_storage : text_storage {
  trk_t persist(uid_t uid, const std::string &hash_name) {
    auto trk = make_trk__(uid, hash_name);

    if (stage == __1) {
      if (stage_2__.contains(trk)) {
        stage_1__.insert(stage_2__.extract(trk));
      } else if (not stage_1__.contains(trk)) {
        stage_1__.insert(
            {trk, text::buffer{std::unique_ptr<text::tokenizer>{new tkk{}},
                               std::pmr::get_default_resource()}});
      }
    } else {
      if (stage_1__.contains(trk)) {
        stage_2__.insert(stage_1__.extract(trk));
      } else if (not stage_2__.contains(trk)) {
        stage_2__.insert(
            {trk, text::buffer{std::unique_ptr<text::tokenizer>{new tkk{}},
                               std::pmr::get_default_resource()}});
      }
    };

    return trk;
  };

  //  trk_t persist(uid_t uid, const std::string &hash_name, std::string
  //  &&init);
};

struct managed_text_storage : mutable_text_storage {

  text::buffer *get(trk_t trk) {
    if (stage == __1) {
      if (stage_1__.contains(trk)) {
        return &stage_1__.at(trk);
      }
    } else {
      if (stage_2__.contains(trk)) {
        return &stage_2__.at(trk);
      }
    }

    return nullptr;
  }

  void advance_generation() {
    if (stage == __1) {
      for (auto &[_, buff] : stage_2__) {
        buff.~buffer();
      }
      stage_2__.clear();

      stage = __2;
    } else {
      for (auto &[_, buff] : stage_1__) {
        buff.~buffer();
      }

      stage_1__.clear();

      stage = __1;
    }
  };
};
}; // namespace iuic
