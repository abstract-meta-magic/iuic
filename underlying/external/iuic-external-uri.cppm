// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:uri;
import std;

export namespace iuic::external {
struct uri {
  constexpr uri(std::string_view str) {
    auto it = str.find("://");
    it += 3;
    data = str;
    prefix_end = it;
    path_size = str.size() - prefix_end;
  }

  constexpr uri(std::string_view scheme, std::string_view path) {}

  constexpr std::string_view get_scheme() const {
    return {&data[0], prefix_end};
  };

  constexpr std::string_view get_path() const {
    return {&data[0] + prefix_end, path_size};
  };

private:
  // nod
  std::string data;
  std::uint16_t prefix_end{0}; // name + ://
  std::uint16_t path_size{0};
};

}; // namespace iuic::external
