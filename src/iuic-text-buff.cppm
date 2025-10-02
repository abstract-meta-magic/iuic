

module;

#include <cstring>
#include <memory>
#include <memory_resource>
#include <string>
#include <utility>
#include <vector>

export module iuic.core:text.buff;
import :text.token;

export namespace iuic::text {
// Хранить в предстовлении мета информацию
class buffer {
public:
  // base ctor
  buffer(std::unique_ptr<tokenizer> &&fmt_, std::pmr::memory_resource *res)
      : fmt{std::move(fmt_)}, resource{res} {};

  ~buffer() {
    if (data) {
      resource->deallocate(data, capacity_);
    }
  };

  buffer(buffer &&) = default;
  buffer &operator=(buffer &&other) = default;

public: // modify api
  // allocate new clear buff
  void reallocate(std::size_t size) {

    char *ndata = (char *)resource->allocate(size + 1);

    if (ndata) {
      if (data != nullptr) {
        resource->deallocate((void *)data, capacity_);
      }

      data = ndata;
      size_ = 1;
      capacity_ = size + 1;
      *(data + size_) = '\0';
    }
  };

  void clear() { size_ = 1; };

  // do nothin
  bool replace(const std::string &str) {
    if (str.size() > capacity_ - 1) {
      return false;
    }

    std::memcpy(data, str.c_str(), str.size());

    size_ = str.size() + 1;
    data[size_] = '\0';
    format__();
    return true;
  };

  bool insert_end(const std::string &str) {
    if (not is_size_avilable(str.size())) {
      return false;
    }

    std::memcpy(data + size_, str.c_str(), str.size());

    size_ += str.size();
    data[size_] = '\0';
    format__();
    return true;
  };

  bool insert_begin(const std::string &str) {
    if (not is_size_avilable(str.size())) {
      return false;
    }

    if (size_ > 1) {
      std::memmove(data + str.size(), data, size_);
    }

    std::memcpy(data, str.c_str(), str.size());

    size_ += str.size();
    data[size_] = '\0';
    format__();
    return true;
  };

  bool insert_at(size_t index, const std::string &str) {
    if (not is_size_avilable(str.size())) {
      return false;
    }

    if (index > size_) {
      return false;
    }

    std::memmove(data + index + str.size(), data + index, size_ - index);

    std::memcpy(data + index, str.c_str(), str.size());

    size_ += str.size();
    data[size_] = '\0';
    format__();
    return true;
  };

  bool is_size_avilable(size_t size) {
    return data && size < (capacity_ - size_);
  };

public: // other api
  void set_tokenizer(std::unique_ptr<tokenizer> &&tokenizer) {
    std::swap(fmt, tokenizer);
  };

  std::string_view get_text() { return {data, size_}; };

  const std::vector<token> &get_formated_text();

  size_t capacity() const noexcept { return capacity_ - 1; };

  size_t size() const noexcept { return size_ - 1; };

  // bool is_dirty() const noexcept;
private:
  void format__() { fmt_text = fmt->proccess(get_text()); };

private:
  // basic buff , mb use SSO ?
  char *data{nullptr};
  //  char *swap{nullptr}; // !!
  size_t size_{0};
  size_t capacity_{0};

  std::pmr::memory_resource *resource;

  // pmr::allocator |
  std::unique_ptr<tokenizer> fmt;
  std::vector<token> fmt_text;
};
}; // namespace iuic::text
