

module;

#include <string_view>
#include <vector>

export module iuic.core:text.token;

export namespace iuic::text {

// описать назначение
struct token {
  struct info;

  struct sequence;

  static const info &default_token_info();

  std::string_view text{"NULL"};

  const info &metadata{default_token_info()};
};

struct token::info {
  std::string_view token_type{"null"};
  std::string_view text_type{"null"};
  std::string_view doc{"null"};
} constexpr default_token_info{.token_type = "short-line",
                               .text_type = "normal",
                               .doc = "This is a simple text line."};

const token::info &token::default_token_info() {
  return text::default_token_info;
};

struct token::sequence {
  // ...
  std::vector<token> tokens{};
};

struct tokenizer {
  virtual ~tokenizer() = default;

  virtual token::sequence process(std::string_view text) const noexcept = 0;
};
}; // namespace iuic::text
