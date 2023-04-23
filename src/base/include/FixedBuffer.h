#ifndef SRC_BASE_INCLUDE_FIXEDBUFFER_H_
#define SRC_BASE_INCLUDE_FIXEDBUFFER_H_

#include <cstring>
#include <string>

namespace TinyWeb {
namespace base {
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer {
 public:
  FixedBuffer() : cur_(data_) {}

  void append(const char* buf, size_t len) {
    if (static_cast<size_t>(avail()) > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }

  const char* data() const { return data_; }
  int length() const { return static_cast<int>(cur_ - data_); }

  char* current() { return cur_; }
  int avail() const { return static_cast<int>(end() - cur_); }
  void add(size_t len) { cur_ += len; }

  void reset() { cur_ = data_; }
  void bzero() { ::bzero(data_, sizeof(data_)); }

  std::string toString() const { return std::string(data_, length()); }

 private:
  const char* end() const { return data_ + sizeof(data_); }

  char data_[SIZE];
  char* cur_;
};
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_FIXEDBUFFER_H_