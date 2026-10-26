// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:present;
import iuic.underlying;
import std;
import :base;

namespace iuic::text {

export struct placed {
  gid id;
  units::pixel offset;
};

struct present_sequence {
  struct range_t {
    std::size_t begin;
    std::size_t end;
  };
  struct pdata {
    range_t range;
    units::ui::position baseline;
    units::upixel height;
    sgid semantic_group;
    lsgid logical_group;
    eid expansion;
  };
  struct edata {
    range_t range;
  };

  std::vector<placed> placement_glyph_sequence;
  std::vector<pdata> placements;
  std::vector<gid> expansion_glyph_sequence;
  std::vector<edata> expansions;
};

export struct placement {

  placement(const present_sequence *seq_, std::size_t index_)
      : seq{seq_}, index{index_} {};

  struct range_t {
    range_t(const present_sequence *seq_, std::size_t index_)
        : seq{seq_}, index{index_} {};
    struct iterator {
      iterator(const present_sequence *seq_, std::size_t index_)
          : seq{seq_}, index{index_} {};

      placed operator*() { return seq->placement_glyph_sequence[index]; };

      iterator &operator++() {
        ++index;
        return *this;
      };

      bool operator==(const iterator &other) const {
        return index == other.index;
      };

    private:
      const present_sequence *seq;
      std::size_t index;
    };

    iterator begin() {
      return iterator{seq, seq->placements[index].range.begin};
    };
    iterator end() { return iterator{seq, seq->placements[index].range.end}; };

  private:
    const present_sequence *seq;
    std::size_t index;
  };
  struct index_range_t {
    index_range_t(const present_sequence *seq_, std::size_t index_)
        : seq{seq_}, index{index_} {};
    struct iterator {
      iterator(std::size_t index_) : index{index_} {};

      std::size_t operator*() { return index; };

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

    iterator begin() { return iterator{seq->placements[index].range.begin}; };
    iterator end() { return iterator{seq->placements[index].range.end}; };

  private:
    const present_sequence *seq;
    std::size_t index;
  };

  units::ui::position get_baseline() const {
    return seq->placements[index].baseline;
  };

  units::upixel get_height() const { return seq->placements[index].height; };

  sgid get_semantic_group() const {
    return seq->placements[index].semantic_group;
  };

  lsgid get_logical_group() const {
    return seq->placements[index].logical_group;
  };

  eid get_expansion() const { return seq->placements[index].expansion; };

  range_t range();

  index_range_t index_range();

private:
  const present_sequence *seq;
  std::size_t index;
};

export struct present {
  struct tool;

  struct placed_proxy {
    placed_proxy(const present_sequence *seq_) : seq{seq_} {};

    struct range_t {
      range_t(const present_sequence *seq_) : seq{seq_} {};

      struct iterator {
        iterator(const present_sequence *seq_, std::size_t index_)
            : seq{seq_}, index{index_} {};

        placed operator*() { return seq->placement_glyph_sequence[index]; };

        iterator &operator++() {
          ++index;
          return *this;
        };

        bool operator==(const iterator &other) const {
          return index == other.index;
        };

      private:
        const present_sequence *seq;
        std::size_t index;
      };

      iterator begin() { return iterator{seq, 0}; };

      iterator end() {
        return iterator{seq, seq->placement_glyph_sequence.size()};
      };

    private:
      const present_sequence *seq;
    };
    struct context_range_t {

      context_range_t(const present_sequence *seq_) : seq{seq_} {};
      struct iterator {
        iterator(const present_sequence *seq_, std::size_t pd_index_,
                 std::size_t pl_index_)
            : seq{seq_}, pd_index{pd_index_}, pl_index{pl_index_} {};

        std::pair<placed, placement> operator*() {
          return std::pair{seq->placement_glyph_sequence[pd_index],
                           placement{seq, pl_index}};
        };

        iterator operator++() {
          ++pd_index;
          if (pd_index <= seq->placements[pl_index].range.end) {
            ++pl_index;
          };
          return *this;
        };

        bool operator==(const iterator &other) const {
          return pd_index == other.pd_index;
        };

      private:
        const present_sequence *seq;
        std::size_t pd_index;
        std::size_t pl_index;
      };

      iterator begin() { return iterator{seq, 0, 0}; };

      iterator end() {
        return iterator{seq, seq->placement_glyph_sequence.size(),
                        seq->placements.size()};
      };

    private:
      const present_sequence *seq;
    };
    struct index_range_t {
      index_range_t(const present_sequence *seq_) : seq{seq_} {};

      struct iterator {
        iterator(std::size_t index_) : index{index_} {};

        std::size_t operator*() { return index; };

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
      iterator end() { return iterator{seq->placement_glyph_sequence.size()}; };

    private:
      const present_sequence *seq;
    };

    struct context_index_range_t {

      context_index_range_t(const present_sequence *seq_) : seq{seq_} {};
      struct iterator {
        iterator(const present_sequence *seq_, std::size_t pd_index_,
                 std::size_t pl_index_)
            : seq{seq_}, pd_index{pd_index_}, pl_index{pl_index_} {};

        std::pair<std::size_t, std::size_t> operator*() {
          return std::pair{pd_index, pl_index};
        };

        iterator operator++() {
          ++pd_index;
          if (pd_index <= seq->placements[pl_index].range.end) {
            ++pl_index;
          };
          return *this;
        };

        bool operator==(const iterator &other) const {
          return pd_index == other.pd_index;
        };

      private:
        const present_sequence *seq;
        std::size_t pd_index;
        std::size_t pl_index;
      };

      iterator begin() { return iterator{seq, 0, 0}; };

      iterator end() {
        return iterator{seq, seq->placement_glyph_sequence.size(),
                        seq->placements.size()};
      };

    private:
      const present_sequence *seq;
    };

    range_t range() { return range_t{seq}; };
    context_range_t context_range() { return context_range_t{seq}; };
    index_range_t index_range() { return index_range_t{seq}; };
    context_index_range_t context_index_range() {
      return context_index_range_t{seq};
    };

    placed at(std::size_t I) { return seq->placement_glyph_sequence[I]; };
    std::size_t size() const { return seq->placement_glyph_sequence.size(); };

  private:
    const present_sequence *seq;
  };

  struct placement_proxy {
    placement_proxy(const present_sequence *seq_) : seq{seq_} {};

    struct range_t {
      range_t(const present_sequence *seq_) : seq{seq_} {};

      struct iterator {
        iterator(const present_sequence *seq_, std::size_t index_)
            : seq{seq_}, index{index_} {};

        placement operator*() { return placement{seq, index}; };

        iterator &operator++() {
          ++index;
          return *this;
        };

        bool operator==(const iterator &other) const {
          return index == other.index;
        };

      private:
        const present_sequence *seq;
        std::size_t index;
      };

      iterator begin() { return iterator{seq, 0}; };

      iterator end() { return iterator{seq, seq->placements.size()}; };

    private:
      const present_sequence *seq;
    };

    struct index_range_t {
      index_range_t(const present_sequence *seq_) : seq{seq_} {};

      struct iterator {
        iterator(std::size_t index_) : index{index_} {};

        std::size_t operator*() { return index; };

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

      iterator end() { return iterator{seq->placements.size()}; };

    private:
      const present_sequence *seq;
    };

    range_t range() { return range_t{seq}; };

    index_range_t index_range() { return index_range_t{seq}; };

    placement at(std::size_t I) { return placement{seq, I}; };

    std::size_t size() const { return seq->placements.size(); };

  private:
    const present_sequence *seq;
  };

  present(present_sequence &&seq_)
      : seq{std::make_shared<present_sequence>(std::move(seq_))} {}

  placed_proxy by_placeds() const { return placed_proxy{seq.get()}; };

  placement_proxy by_placements() const { return placement_proxy{seq.get()}; };

  // eproxy by_expansions

private:
  std::shared_ptr<present_sequence> seq;
};
}; // namespace iuic::text
