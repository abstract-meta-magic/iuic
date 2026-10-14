// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text;
import iuic.underlying;
import iuic.asset;
import std;

namespace iuic::text {

export namespace semantic {
struct decl {
  consteval decl() : self{this} {}
  const decl *self; // force static
};

struct value {
  const decl *decl;
};
}; // namespace semantic

export enum class glyph : std::uint64_t {};

export struct decoder;

export struct glyph_info {
  // fields
};

// Модеть текста
export struct model {

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

  const glyph_info &at(glyph g) const {
    std::size_t index{std::to_underlying(g)};
    if (infos.size() < index) {
      return infos.at(index_map.at(index));
    } else {
      // TODO: normal fallback ?
      throw std::runtime_error{""};
    }
  };

private:
  std::vector<glyph_info> infos;
  std::vector<std::size_t> index_map;
};

export struct segment {
  semantic::value semantic;
  std::span<const glyph> glyphs;
};

export struct decoded {
  // Отвечает за преобразование текста согласно модели
  // так же, наверное он должет отвечать за правила разделения\разбиения
  // и симантические признаки

  // liner memory contains -> | glyphs.. | segments.. | decoded |
  struct memory_owner {

    constexpr memory_owner(
        const std::byte *data_, std::size_t size_,
        std::invocable<const std::byte *, std::size_t> auto &&deallocator_)
        : data{data_}, size{size_},
          deallocator{std::forward<decltype(deallocator_)>(deallocator_)} {}

    constexpr memory_owner(const memory_owner &) = delete;
    constexpr memory_owner &operator=(const memory_owner &) = delete;
    constexpr memory_owner(memory_owner &&other)
        : data{nullptr}, size{0}, deallocator{nullptr} {
      std::swap(data, other.data);
      std::swap(size, other.size);
      std::swap(deallocator, other.deallocator);
    };
    constexpr memory_owner &operator=(memory_owner &&other) {
      if (this != &other) {
        std::swap(data, other.data);
        std::swap(size, other.size);
        std::swap(deallocator, other.deallocator);
      }
      return *this;
    };
    constexpr ~memory_owner() {
      if (data && deallocator) {
        deallocator(data, size);
      }
    };

  private:
    const std::byte *data;
    std::size_t size;
    std::function<void(const std::byte *, std::size_t)> deallocator;
  };

  std::span<const segment> value;

  std::reference_wrapper<const decoder> decoder;

  std::variant<asset::declaration, asset::static_instance, asset::instance_id>
      asset;
};

export struct decoder {
  enum class err {
    unsupportetd,
    /// ...
  };

  constexpr virtual ~decoder() {};

  constexpr virtual std::expected<std::pair<decoded, decoded::memory_owner>,
                                  err>
  decode(std::string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual std::expected<std::pair<decoded, decoded::memory_owner>,
                                  err>
  decode_utf8(std::u8string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual std::expected<std::pair<decoded, decoded::memory_owner>,
                                  err>
  decode_utf16(std::u16string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual std::expected<std::pair<decoded, decoded::memory_owner>,
                                  err>
  decode_utf32(std::u32string_view) const {
    return std::unexpected{err::unsupportetd};
  };

  constexpr virtual const model &get_model() const = 0;
};

export struct placed {
  glyph id;
  units::pixel offset;
};

export struct placement {
  semantic::value semantic;
  units::upixel height;
  std::span<const placed> glyphs;
};

export struct present {
  // memory owner ???
  std::span<const placement> placements;

  std::variant<asset::declaration, asset::static_instance, asset::instance_id>
      asset;
};

export struct font {
  std::reference_wrapper<const decoded> decoder;
  // info ...
};

} // namespace iuic::text
