// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:model.decoder;
import std;
import :base;
import :model;
import :decoded;

namespace iuic::text {

struct model::decoder {
private:
  // private impl interface
  // and shared ptr to impl
  struct impl {};

public:
  enum class err {
    unsupportetd,
    /// ...
  };

  constexpr virtual ~decoder() {};

  constexpr virtual std::expected<decoded, err> decode(std::string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual std::expected<decoded, err>
  decode_utf8(std::u8string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual std::expected<decoded, err>
  decode_utf16(std::u16string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual std::expected<decoded, err>
  decode_utf32(std::u32string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  model of() const { return model_; };

private:
  model model_;
};

}; // namespace iuic::text
