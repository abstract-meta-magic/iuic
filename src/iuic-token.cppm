

module;

#include <string_view>
#include <vector>

export module iuic.core:text.token;

export namespace iuic::text {

struct token {
  std::string_view text;
  enum {
    NORMAL,
    BOLD,
    // ..
  } type;
  // ..
};

struct tokenizer {
  virtual ~tokenizer() = default;

  virtual std::vector<token> proccess(std::string_view text) const noexcept = 0;
};
}; // namespace iuic::text
