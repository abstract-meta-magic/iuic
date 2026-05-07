// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.units:op;
import :decl;

namespace iuic::units {
template <typename T> struct expressin_result {};
template <auto value> struct consteval_expressin_result {};
}; // namespace iuic::units

export namespace iuic::units {

constexpr inline uid &operator++(uid &self) {
  self = uid{std::to_underlying(self) + 1};
  return self;
}

constexpr inline uid operator++(uid &self, int) {
  auto res = self;
  ++self;
  return res;
}

constexpr inline auto operator<=>(percent lhs, percent rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};
constexpr inline auto operator<=>(vh lhs, vh rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};
constexpr inline auto operator<=>(vw lhs, vw rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline auto operator<=>(pixel lhs, pixel rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline auto operator<=>(upixel lhs, upixel rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline auto operator<=>(segment lhs, segment rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline auto operator<=>(weight lhs, weight rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline auto operator<=>(uid lhs, uid rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline auto operator<=>(hash lhs, hash rhs) {
  return std::to_underlying(lhs) <=> std::to_underlying(rhs);
};

constexpr inline percent operator+(percent lhs, percent rhs) {
  // TODO : Write overflow rule
  if (255 - static_cast<std::uint8_t>(lhs) > static_cast<std::uint8_t>(rhs)) {
    return percent{255};
  } else {
    return percent{static_cast<std::uint8_t>(std::to_underlying(lhs) +
                                             std::to_underlying(rhs))};
  }
}

constexpr inline percent operator-(percent lhs, percent rhs) {
  // TODO : Write overflow rule
  if (std::to_underlying(lhs) < std::to_underlying(rhs)) {
    return percent{0};
  } else {
    return percent{static_cast<std::uint8_t>(std::to_underlying(lhs) -
                                             std::to_underlying(rhs))};
  };
}

constexpr inline segment operator+(segment lhs, segment rhs) {
  return segment{std::to_underlying(lhs) + std::to_underlying(rhs)};
}

constexpr inline segment operator-(segment lhs, segment rhs) {
  return segment{std::to_underlying(lhs) - std::to_underlying(rhs)};
}

constexpr inline upixel operator*(segment lhs, upixel rhs) {
  return upixel{std::to_underlying(lhs) * std::to_underlying(rhs)};
}

constexpr inline upixel operator*(upixel lhs, segment rhs) {
  return upixel{std::to_underlying(lhs) * std::to_underlying(rhs)};
}

constexpr inline pixel operator+(pixel lhs, pixel rhs) {
  return pixel{std::to_underlying(lhs) + std::to_underlying(rhs)};
}

constexpr inline pixel operator-(pixel lhs, pixel rhs) {
  return pixel{std::to_underlying(lhs) - std::to_underlying(rhs)};
}

constexpr inline pixel operator*(pixel lhs, std::uint8_t rhs) {
  // TODO : Write overflow rule
  return pixel{std::to_underlying(lhs) * rhs};
}

constexpr inline pixel operator*(pixel lhs, percent rhs) {
  // TODO : Write overflow rule
  return pixel{static_cast<std::int32_t>(static_cast<std::int64_t>(lhs) *
                                         std::to_underlying(rhs) / 100)};
}

constexpr inline pixel operator/(pixel lhs, std::uint8_t rhs) {
  // TODO : Write Nolmalization rule
  return pixel{std::to_underlying(lhs) / rhs};
}

constexpr inline pixel operator%(pixel lhs, std::uint8_t rhs) {
  return pixel{std::to_underlying(lhs) % rhs};
}

constexpr inline upixel operator+(upixel lhs, upixel rhs) {
  return upixel{std::to_underlying(lhs) + std::to_underlying(rhs)};
}

constexpr inline upixel operator-(upixel lhs, upixel rhs) {
  return upixel{std::to_underlying(lhs) - std::to_underlying(rhs)};
}

constexpr inline upixel operator*(upixel lhs, std::uint8_t rhs) {
  // TODO : Write overflow rule
  return upixel{std::to_underlying(lhs) * rhs};
}

constexpr inline upixel operator*(upixel lhs, std::size_t rhs) {
  // TODO : Write overflow rule
  return upixel{static_cast<std::underlying_type_t<upixel>>(
      std::to_underlying(lhs) * rhs)};
}

constexpr inline upixel operator*(upixel lhs, percent rhs) {
  // TODO : Write overflow rule
  return upixel{static_cast<std::uint32_t>(static_cast<std::uint64_t>(lhs) *
                                           std::to_underlying(rhs) / 100)};
}

constexpr inline upixel operator*(upixel lhs, vh rhs) {
  // TODO : Write overflow rule
  return upixel{std::to_underlying(lhs) * std::to_underlying(rhs)};
}
constexpr inline upixel operator*(upixel lhs, vw rhs) {
  // TODO : Write overflow rule
  return upixel{std::to_underlying(lhs) * std::to_underlying(rhs)};
}

constexpr inline upixel operator/(upixel lhs, std::uint8_t rhs) {
  // TODO : Write Nolmalization rule
  return upixel{std::to_underlying(lhs) / rhs};
}

constexpr inline weight operator+(weight lhs, weight rhs) {
  return weight{std::to_underlying(lhs) + std::to_underlying(rhs)};
}
constexpr inline weight operator-(weight lhs, weight rhs) {
  return weight{std::to_underlying(lhs) - std::to_underlying(rhs)};
}

constexpr inline upixel operator%(upixel lhs, std::uint8_t rhs) {
  return upixel{std::to_underlying(lhs) % rhs};
}

constexpr inline pixel operator+(pixel lhs, upixel rhs) {
  return pixel{static_cast<std::underlying_type_t<pixel>>(
      std::to_underlying(lhs) + std::to_underlying(rhs))};
}

constexpr inline pixel operator-(pixel lhs, upixel rhs) {
  return pixel{static_cast<std::underlying_type_t<pixel>>(
      std::to_underlying(lhs) - std::to_underlying(rhs))};
}

constexpr inline pixel operator+(upixel lhs, pixel rhs) {
  return pixel{static_cast<std::underlying_type_t<pixel>>(
      std::to_underlying(lhs) + std::to_underlying(rhs))};
}

constexpr inline pixel operator-(upixel lhs, pixel rhs) {
  return pixel{static_cast<std::underlying_type_t<pixel>>(
      std::to_underlying(lhs) - std::to_underlying(rhs))};
}

constexpr inline upixel operator/(upixel free, weight sum) {
  return upixel{static_cast<std::underlying_type_t<upixel>>(
      static_cast<std::uint64_t>(free) * 1000 / std::to_underlying(sum))};
}

constexpr inline upixel operator*(upixel unit, weight w) {
  return upixel{static_cast<std::underlying_type_t<upixel>>(
      static_cast<std::uint64_t>(unit) * std::to_underlying(w) / 1000)};
}

}; // namespace iuic::units
