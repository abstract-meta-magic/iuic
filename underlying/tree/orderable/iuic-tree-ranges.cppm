// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree.orderable:ranges;
import std;
import iuic.underlying.iterator;
import :impl;

namespace iuic::iterator::range {

template <typename T, typename index_t>
struct spec<iuic::tree::orderable<T, index_t> &,
            iuic::iterator::range::basic_t> {
  struct range {
    range(iuic::iterator::basic<iuic::tree::orderable<T, index_t>> it)
        : begin_{it} {};

    iuic::iterator::access<iuic::tree::orderable<T, index_t>> begin() {
      return begin_;
    };

    iuic::iterator::sentinel<iuic::tree::orderable<T, index_t>> end() {
      return begin_;
    };

  private:
    iuic::iterator::basic<iuic::tree::orderable<T, index_t>> begin_;
  };

  static range make(iuic::tree::orderable<T, index_t> &container,
                    iuic::iterator::range::basic_t) {
    return range{container.begin()};
  };
};

template <typename T, typename index_t>
struct spec<const iuic::tree::orderable<T, index_t> &,
            iuic::iterator::range::basic_t> {
  struct range {
    range(iuic::iterator::basic<iuic::tree::orderable<T, index_t>> it)
        : begin_{it} {};

    iuic::iterator::const_access<iuic::tree::orderable<T, index_t>> begin() {
      return begin_;
    };

    iuic::iterator::sentinel<iuic::tree::orderable<T, index_t>> end() {
      return begin_;
    };

  private:
    iuic::iterator::basic<iuic::tree::orderable<T, index_t>> begin_;
  };

  static range make(const iuic::tree::orderable<T, index_t> &container,
                    iuic::iterator::range::reverse_t) {
    return range{container.begin()};
  };
};

template <typename T, typename index_t>
struct spec<const iuic::tree::orderable<T, index_t> &,
            iuic::iterator::range::reverse_t> {
  struct range {
    range(iuic::iterator::basic<iuic::tree::orderable<T, index_t>,
                                iuic::iterator::mode::reverse_t>
              it)
        : begin_{it} {};

    iuic::iterator::const_access<iuic::tree::orderable<T, index_t>,
                                 iuic::iterator::mode::reverse_t>
    begin() {
      return begin_;
    };

    iuic::iterator::sentinel<iuic::tree::orderable<T, index_t>,
                             iuic::iterator::mode::reverse_t>
    end() {
      return begin_;
    };

  private:
    iuic::iterator::basic<iuic::tree::orderable<T, index_t>,
                          iuic::iterator::mode::reverse_t>
        begin_;
  };

  static range make(const iuic::tree::orderable<T, index_t> &container,
                    iuic::iterator::range::reverse_t) {
    return range{--container.end()};
  };
};

template <typename T, typename index_t>
struct spec<iuic::tree::orderable<T, index_t> &,
            iuic::iterator::range::reverse_t> {
  struct range {
    range(iuic::iterator::basic<iuic::tree::orderable<T, index_t>,
                                iuic::iterator::mode::reverse_t>
              it)
        : begin_{it} {};

    iuic::iterator::access<iuic::tree::orderable<T, index_t>,
                           iuic::iterator::mode::reverse_t>
    begin() {
      return begin_;
    };

    iuic::iterator::sentinel<iuic::tree::orderable<T, index_t>,
                             iuic::iterator::mode::reverse_t>
    end() {
      return begin_;
    };

  private:
    iuic::iterator::basic<iuic::tree::orderable<T, index_t>,
                          iuic::iterator::mode::reverse_t>
        begin_;
  };
  static range make(iuic::tree::orderable<T, index_t> &container,

                    iuic::iterator::range::reverse_t) {
    return range{--container.end()};
  };
};

}; // namespace iuic::iterator::range
