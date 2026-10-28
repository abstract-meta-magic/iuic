// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:model.decoder;
import std;
import :base;
import :model;
import :decoded;

namespace iuic::text {

template <typename T>
concept has_basic_decode = requires(T obj, std::string_view str) {
  { obj.decode(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::string_view str) {
  { obj(str) } -> std::convertible_to<decoded>;
};

template <typename T>
concept has_utf8_decode = requires(T obj, std::u8string_view str) {
  { obj.decode_utf8(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::u8string_view str) {
  { obj.decode(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::u8string_view str) {
  { obj(str) } -> std::convertible_to<decoded>;
};

template <typename T>
concept has_utf16_decode = requires(T obj, std::u16string_view str) {
  { obj.decode_utf16(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::u16string_view str) {
  { obj.decode(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::u16string_view str) {
  { obj(str) } -> std::convertible_to<decoded>;
};

template <typename T>
concept has_utf32_decode = requires(T obj, std::u32string_view str) {
  { obj.decode_utf32(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::u32string_view str) {
  { obj.decode(str) } -> std::convertible_to<decoded>;
} || requires(T obj, std::u32string_view str) {
  { obj(str) } -> std::convertible_to<decoded>;
};

template <typename T>
concept is_decoder_impl = has_basic_decode<T> || has_utf8_decode<T> ||
                          has_utf16_decode<T> || has_utf32_decode<T>;

struct model::decoder {
  enum class err {
    unsupported,
    /// ...
  };

private:
  // private impl interface
  // and shared ptr to impl
  struct impl {
    constexpr virtual ~impl() {};

    constexpr virtual std::expected<decoded, err>
    decode(std::string_view) const {
      return std::unexpected{err::unsupported};
    };

    constexpr virtual std::expected<decoded, err>
    decode_utf8(std::u8string_view) const {
      return std::unexpected{err::unsupported};
    };

    constexpr virtual std::expected<decoded, err>
    decode_utf16(std::u16string_view) const {
      return std::unexpected{err::unsupported};
    };

    constexpr virtual std::expected<decoded, err>
    decode_utf32(std::u32string_view) const {
      return std::unexpected{err::unsupported};
    };
  };

  static std::shared_ptr<impl> make_impl(is_decoder_impl auto &&obj) {
    struct hide_impl : impl {
      using value_type = std::remove_cvref_t<decltype(obj)>;

      hide_impl(decltype(std::forward<decltype(obj)>(obj)) obj_)
          : value{std::forward<decltype(obj_)>(obj_)} {};

      constexpr std::expected<decoded, err>
      decode(std::string_view str) const override {
        if constexpr (requires { value.decode(str); }) {
          return value.decode(str);
        } else if constexpr (requires { value(str); }) {
          return value(str);
        } else {
          return std::unexpected{err::unsupported};
        }
      };

      constexpr std::expected<decoded, err>
      decode_utf8(std::u8string_view str) const override {
        if constexpr (requires { value.decode(str); }) {
          return value.decode(str);
        } else if constexpr (requires { value.decode_utf8(str); }) {
          return value.decode_utf8(str);
        } else if constexpr (requires { value(str); }) {
          return value(str);
        } else {
          return std::unexpected{err::unsupported};
        }
      };

      constexpr std::expected<decoded, err>
      decode_utf16(std::u16string_view str) const override {
        if constexpr (requires { value.decode(str); }) {
          return value.decode(str);
        } else if constexpr (requires { value.decode_utf16(str); }) {
          return value.decode_utf16(str);
        } else if constexpr (requires { value(str); }) {
          return value(str);
        } else {
          return std::unexpected{err::unsupported};
        }
      };

      constexpr std::expected<decoded, err>
      decode_utf32(std::u32string_view str) const override {
        if constexpr (requires { value.decode(str); }) {
          return value.decode(str);
        } else if constexpr (requires { value.decode_utf32(str); }) {
          return value.decode_utf32(str);
        } else if constexpr (requires { value(str); }) {
          return value(str);
        } else {
          return std::unexpected{err::unsupported};
        }
      };

      value_type value;
    };

    return std::shared_ptr<impl>(
        new hide_impl{std::forward<decltype(obj)>(obj)});
  };

  static std::shared_ptr<impl> make_null_impl() {
    struct hide_impl : impl {

      constexpr std::expected<decoded, err>
      decode(std::string_view str) const override {
        return std::unexpected{err::unsupported};
      };

      constexpr std::expected<decoded, err>
      decode_utf8(std::u8string_view str) const override {
        return std::unexpected{err::unsupported};
      };

      constexpr std::expected<decoded, err>
      decode_utf16(std::u16string_view str) const override {
        return std::unexpected{err::unsupported};
      };

      constexpr std::expected<decoded, err>
      decode_utf32(std::u32string_view str) const override {
        return std::unexpected{err::unsupported};
      };
    };
    return std::shared_ptr<impl>(new hide_impl{});
  };

public:
  decoder(model m, is_decoder_impl auto &&obj)
      : model_{m}, pimpl{make_impl(std::forward<decltype(obj)>(obj))} {};

  decoder(model m) : model_{m}, pimpl{make_null_impl()} {};
  decoder() : model_{}, pimpl{make_null_impl()} {};

  constexpr std::expected<decoded, err> decode(std::string_view str) const {
    return pimpl->decode(str);
  };

  constexpr std::expected<decoded, err>
  decode_utf8(std::u8string_view str) const {
    return pimpl->decode_utf8(str);
  };

  constexpr std::expected<decoded, err>
  decode_utf16(std::u16string_view str) const {
    return pimpl->decode_utf16(str);
  };

  constexpr std::expected<decoded, err>
  decode_utf32(std::u32string_view str) const {
    return pimpl->decode_utf32(str);
  };

  model of() const { return model_; };

public: // BIG-V
  decoder(decoder &&other) noexcept
      : model_{other.model_}, pimpl{std::move(other.pimpl)} {};

  decoder &operator=(decoder &&other) noexcept {
    std::swap(model_, other.model_);
    std::swap(pimpl, other.pimpl);
    return *this;
  };

  decoder(const decoder &other) noexcept
      : model_{other.model_}, pimpl{other.pimpl} {};

  decoder &operator=(const decoder &other) noexcept {
    if (std::addressof(other) == this)
      return *this;

    model_ = other.model_;
    pimpl = other.pimpl;

    return *this;
  };

private:
  model model_;
  std::shared_ptr<impl> pimpl;
};

}; // namespace iuic::text
