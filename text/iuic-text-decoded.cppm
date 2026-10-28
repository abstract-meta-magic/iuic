// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:decoded;
import iuic.underlying;
import std;
import :base;

namespace iuic::text {

struct decoded_sequence {
  // Segments
  struct sdata {
    lsgid group;
    sgid semantic;
    eid expansion;
    std::size_t begin;
    std::size_t end;
  };

  // Expansions
  struct edata {
    std::size_t begin;
    std::size_t end;
    // later more data
  };

  std::vector<gcid> segments_context_sequence;
  std::vector<sdata> segments;
  std::vector<gcid> expansions_context_sequence;
  std::vector<edata> expansions;
};

export struct segment {
  segment(const decoded_sequence *seq_, std::size_t index_)
      : seq{seq_}, index{index_} {};

  struct range_t {
    range_t(const decoded_sequence *seq_, std::size_t index_)
        : seq{seq_}, index{index_} {};

    struct iterator {
      iterator(const decoded_sequence *seq_, std::size_t index_)
          : seq{seq_}, index{index_} {};

      gcid operator*() const {
        return seq->segments_context_sequence.at(index);
      };

      iterator &operator++() {
        ++index;
        return *this;
      };

      bool operator==(const iterator &other) const {
        return index == other.index;
      };

    private:
      const decoded_sequence *seq;
      std::size_t index;
    };

    iterator begin() { return iterator{seq, seq->segments[index].begin}; };

    iterator end() { return iterator{seq, seq->segments[index].end}; };

  private:
    const decoded_sequence *seq;
    std::size_t index;
  };

  struct index_range_t {
    index_range_t(const decoded_sequence *seq_, std::size_t index_)
        : seq{seq_}, index{index_} {};

    struct iterator {
      iterator(std::size_t index_) : index{index_} {};

      // return global pos
      std::size_t operator*() const { return index; };

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

    iterator begin() { return iterator{seq->segments[index].begin}; };

    iterator end() { return iterator{seq->segments[index].end}; };

  private:
    const decoded_sequence *seq;
    std::size_t index;
  };

  range_t range() const { return range_t{seq, index}; };

  index_range_t index_range() const { return index_range_t{seq, index}; };

  sgid get_semantic_group() const { return seq->segments[index].semantic; };

  lsgid get_logical_group() const { return seq->segments[index].group; };

  eid get_expansion() const { return seq->segments[index].expansion; };

  gcid at(std::size_t I) const { return seq->segments_context_sequence.at(I); };

  std::size_t size() const {
    return seq->segments[index].end - seq->segments[index].begin;
  };

  // TODO: need local pos ???
  // std::size_t local_position_of(std::size_t g) const;

private:
  const decoded_sequence *seq;
  std::size_t index;
};

export struct decoded {
  decoded(decoded_sequence &&seq_)
      : seq{std::make_shared<decoded_sequence>(std::move(seq_))} {};

  struct memory_owner;
  struct tool;
  struct gproxy {
    gproxy(const decoded_sequence *seq_) : seq{seq_} {};
    struct range_t {
      range_t(const decoded_sequence *seq_) : seq{seq_} {};
      struct iterator {
        iterator(const decoded_sequence *seq_, std::size_t index_)
            : seq{seq_}, index{index_} {};

        gcid operator*() const {
          return seq->segments_context_sequence[index];
        };

        iterator &operator++() {
          ++index;
          return *this;
        };

        bool operator==(const iterator &other) const {
          return index == other.index;
        };

      private:
        const decoded_sequence *seq;
        std::size_t index;
      };

      iterator begin() { return iterator{seq, 0}; };
      iterator end() {
        return iterator{seq, seq->segments_context_sequence.size()};
      };

    private:
      const decoded_sequence *seq;
    };
    struct context_range_t {

      context_range_t(const decoded_sequence *seq_) : seq{seq_} {};
      struct iterator {
        iterator(const decoded_sequence *seq_, std::size_t gindex_,
                 std::size_t sindex_)
            : seq{seq_}, gindex{gindex_}, sindex{sindex_} {};

        std::pair<gcid, segment> operator*() const {
          return std::pair{seq->segments_context_sequence[gindex],
                           segment{seq, sindex}};
        };

        iterator &operator++() {
          ++gindex;
          if (gindex == seq->segments[sindex].end) {
            ++sindex;
          };
          return *this;
        };

        bool operator==(const iterator &other) const {
          return gindex == other.gindex;
        };

      private:
        const decoded_sequence *seq;
        std::size_t gindex;
        std::size_t sindex;
      };

      iterator begin() { return iterator{seq, 0, 0}; };
      iterator end() {
        return iterator{seq, seq->segments_context_sequence.size(),
                        seq->segments.size()};
      };

    private:
      const decoded_sequence *seq;
    };

    struct index_range_t {
      index_range_t(const decoded_sequence *seq_) : seq{seq_} {};
      struct iterator {
        iterator(std::size_t index_) : index{index_} {};
        std::size_t operator*() const { return index; };
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

      iterator begin() { return 0; };
      iterator end() { return seq->segments_context_sequence.size(); };

    private:
      const decoded_sequence *seq;
    };

    struct context_index_range_t {
      context_index_range_t(const decoded_sequence *seq_) : seq{seq_} {};
      struct iterator {

        iterator(const decoded_sequence *seq_, std::size_t gindex_,
                 std::size_t sindex_)
            : seq{seq_}, gindex{gindex_}, sindex{sindex_} {};

        std::pair<std::size_t, std::size_t> operator*() const {
          return std::pair{gindex, sindex};
        };

        iterator &operator++() {
          ++gindex;
          if (gindex == seq->segments[sindex].end) {
            ++sindex;
          };
          return *this;
        };

        bool operator==(const iterator &other) const {
          return gindex == other.gindex;
        };

      private:
        const decoded_sequence *seq;
        std::size_t gindex;
        std::size_t sindex;
      };

      iterator begin() { return iterator{seq, 0, 0}; };
      iterator end() {
        return iterator{seq, seq->segments_context_sequence.size(),
                        seq->segments.size()};
      };

    private:
      const decoded_sequence *seq;
    };

    gcid at(std::size_t I) { return seq->segments_context_sequence.at(I); };
    range_t range() { return range_t{seq}; };
    context_range_t context_range() { return context_range_t{seq}; };
    index_range_t index_range() { return index_range_t{seq}; };
    context_index_range_t context_index_range() {
      return context_index_range_t{seq};
    };
    std::size_t size() { return seq->segments_context_sequence.size(); };

  private:
    const decoded_sequence *seq; // const ?
  };

  struct sproxy {
    sproxy(const decoded_sequence *seq_) : seq{seq_} {};

    struct range_t {
      range_t(const decoded_sequence *seq_) : seq{seq_} {}
      struct iterator {
        iterator(const decoded_sequence *seq_, std::size_t index_)
            : seq{seq_}, index{index_} {};

        iterator &operator++() {
          ++index;
          return *this;
        };

        segment operator*() { return segment{seq, index}; };

        bool operator==(const iterator &other) const {
          return index == other.index;
        };

      private:
        std::size_t index;
        const decoded_sequence *seq;
      };

      iterator begin() { return iterator{seq, 0}; };

      iterator end() { return iterator{seq, seq->segments.size()}; };

    private:
      const decoded_sequence *seq;
    };

    struct index_range_t {
      index_range_t(const decoded_sequence *seq_) : seq{seq_} {}
      struct iterator {
        iterator(std::size_t index_) : index{index_} {};
        iterator &operator++() {
          ++index;
          return *this;
        };

        std::size_t operator*() { return index; };

        bool operator==(const iterator &other) const {
          return index == other.index;
        };

      private:
        std::size_t index;
      };

      iterator begin() { return iterator{0}; };
      iterator end() { return iterator{seq->segments.size()}; };

    private:
      const decoded_sequence *seq;
    };

    segment at(std::size_t I) { return segment{seq, I}; };
    range_t range() const { return range_t{seq}; };
    index_range_t index_range() { return index_range_t{seq}; };
    std::size_t size() const { return seq->segments.size(); };

  private:
    const decoded_sequence *seq; // const ?
  };

  gproxy by_glyphs() const { return gproxy{seq.get()}; };

  sproxy by_segments() const { return sproxy{seq.get()}; };

  // eproxy by_expansions();
private:
  std::shared_ptr<decoded_sequence> seq; // <-- TODO: Make shared ?
};
}; // namespace iuic::text
