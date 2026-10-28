// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:model;
import std;
import :base;

namespace iuic::text {

export struct glyph_info {
  codepoint code;
  // more info
};

namespace metrics {

struct cell {
  static constexpr float height{1.0f};
  float width;
  float baseline;
};

struct layout {
  cmid cell;
  float gap;
};

}; // namespace metrics

export enum class context_type { Dependent, Independent };
// TODO: Rename
export enum class glyph_type { Default, Composite };

struct properties {}; // enum + bitset

// Модеть текста
export struct model {
  /* vector maps
    mid      [...]
    gid      [...]
    type     [...]
    dep      [...]
    links    [...]
    info     [...]
    metrict  [...]
    char_map [...]
  */

  /*
    text: A.V
    seq : [22,44,56] A = 22 , . = 44 , V = 56
    text : AV
    seq : [22,72] A = 22 , V = 72

    56 - обычный символ Independed
    72 - Depended символ
  */

private:
  // Блок единицы модели
  // on prototype - btw shared_ptr
  struct unit_block {
    struct glyph_data {
      giid info{giid::null};
      miid meta{miid::null};
      cid compose{cid::null};
      glyph_type type{glyph_type::Default};
    };

    struct context_data {
      gid glyph{gid::null};
      miid meta{miid::null};
      context_type type{context_type::Independent};
    };

    struct range_ {
      std::size_t begin;
      std::size_t end;
    };

    // info
    std::vector<glyph_info> info;
    std::vector<std::string> meta;

    // compose
    std::vector<gid> compose_sequences; // just mem
    std::vector<range_> compose;

    // metrics
    std::vector<metrics::layout> layout_metrics;
    std::vector<metrics::cell> cell_metrics;

    // main
    std::vector<glyph_data> glyph;
    std::vector<context_data> context;

    // model
    std::string description{"Missing"};
    properties props;

    std::vector<semantic> semantic_sequences; // just mem
    std::vector<range_> semantic;
  };
  using block_t = std::shared_ptr<unit_block>;

  model(block_t block_) : block{std::move(block_)} {};

public:
  struct tool;
  struct decoder;
  struct profile; // additional metrics
  struct glyph_range_t {
    glyph_range_t(std::size_t end) : end_{end} {};

    struct iterator {
      iterator(std::size_t index_) : index{index_} {};

      gid operator*() { return gid{index}; };

      iterator &operator++() {
        ++index;
        return *this;
      };

      bool operator==(const iterator &other) const {
        return index == other.index;
      };

    private:
      std::size_t index;
    };

    iterator begin() { return iterator{0}; };

    iterator end() { return iterator{end_}; };

  private:
    std::size_t end_;
  };

  struct context_range_t {
    context_range_t(std::size_t end) : end_{end} {};
    struct iterator {
      iterator(std::size_t index_) : index{index_} {};

      gcid operator*() { return gcid{index}; };

      iterator &operator++() {
        ++index;
        return *this;
      };

      bool operator==(const iterator &other) const {
        return index == other.index;
      };

    private:
      std::size_t index;
    };

    iterator begin() { return iterator{0}; };

    iterator end() { return iterator{end_}; };

  private:
    std::size_t end_;
  };
  // Направление
  // Абстрактные размеры

  // Модель отвечает на вопросы
  // - Где расположен глиф в абстрактном локальном атласе
  // - Каковы его метрики
  // А что использует эту модель для него не важно
  // FiraCode, JetBrains, Aria , ...

  // Хранит
  // символы
  // где :
  // символ под индексом 55
  // может означать свернутую лигатуру '=='
  // и иметь в info двойные размеры
  // и иные отступы

  // Не определяет симантику
  // А только то, что нужно для
  // шейпинга

  // то чего не существует в модели
  // но должно как-то определяться

  gid unexpected_glyhp() const;

  gid empty_glyhp() const;

  gid glyph_of(gcid c) const {
    return block->context[std::to_underlying(c)].glyph;
  };

  giid info_of(gid g) const {
    return block->glyph[std::to_underlying(g)].info;
  };

  giid info_of(gcid c) const {
    return block
        ->glyph[std::to_underlying(block->context[std::to_underlying(c)].glyph)]
        .info;
  };

  miid meta_of(gid g) const {
    return block->glyph[std::to_underlying(g)].meta;
  };

  miid meta_of(gcid c) const {
    return block->context[std::to_underlying(c)].meta;
  };

  context_type type_of(gcid) const { return context_type::Independent; };

  glyph_type type_of(gid) const { return glyph_type::Default; };

  std::span<const gid> decompose(gid g) const {
    auto cid_ = block->glyph[std::to_underlying(g)].compose;
    if (cid_ == cid::null) {
      return {};
    } else {
      auto &compose_ = block->compose[std::to_underlying(cid_)];
      return std::span{&block->compose_sequences[compose_.begin],
                       compose_.end - compose_.begin};
    }
  };

  std::span<const gid> decompose(gcid c) const {
    return decompose(block->context[std::to_underlying(c)].glyph);
  };

  std::span<const struct semantic> semantic_of(sgid s) const {
    auto srange = block->semantic[std::to_underlying(s)];
    return std::span{&block->semantic_sequences[srange.begin],
                     srange.end - srange.begin};
  };

  glyph_range_t glyph_range() const {
    return glyph_range_t{block->glyph.size()};
  };

  context_range_t context_range() const {
    return context_range_t{block->context.size()};
  };

public: // Access
  const glyph_info &get(giid i) const {
    return block->info[std::to_underlying(i)];
  };

  std::string_view get(miid m) const {
    return block->meta[std::to_underlying(m)];
  };

public: // CTOR
  model() noexcept {};

private:
  block_t block;
};

}; // namespace iuic::text
