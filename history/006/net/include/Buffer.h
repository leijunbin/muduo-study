#ifndef SRC_NET_INCLUDE_BUFFER_H_
#define SRC_NET_INCLUDE_BUFFER_H_

#include <assert.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

namespace TinyWeb {
namespace net {
class Buffer {
 public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;
  // inline static const char tempCRLF[] = "\r\n";

  explicit Buffer(size_t initialSize = kInitialSize)
      : buffer_(kCheapPrepend + initialSize),
        readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend) {}
  Buffer(Buffer &&) = default;
  Buffer &operator=(Buffer &&) = default;

  size_t readableBytes() const { return writerIndex_ - readerIndex_; }
  size_t writeableBytes() const { return buffer_.size() - writerIndex_; }
  size_t prependableBytes() const { return readerIndex_; }

  char *prependPeek() { return begin(); }

  const char *peek() const { return begin() + readerIndex_; }

  char *beginWrite() { return begin() + writerIndex_; }
  const char *beginWrite() const { return begin() + writerIndex_; }

  void retrieve(size_t len) {
    if (len < readableBytes()) {
      readerIndex_ += len;
    } else {
      retrieveAll();
    }
  }
  void retrieveAll() { readerIndex_ = writerIndex_ = kCheapPrepend; }
  std::string retrieveAsString(size_t len) {
    assert(len <= readableBytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
  }

  void ensureWriteableBytes(size_t len) {
    if (writeableBytes() < len) {
      makeSpace(len);
    }
  }
  void append(const char *data, size_t len) {
    ensureWriteableBytes(len);
    std::copy(data, data + len, beginWrite());
    writerIndex_ += len;
  }
  void append(const char *str) { append(str, strlen(str)); }
  void append(const std::string &str) { append(str.c_str(), str.size()); }

  // const char *findCRLF(const char *start = nullptr) const {
  //   if (start != nullptr) {
  //     assert(peek() <= start);
  //     assert(start <= beginWrite());
  //   } else {
  //     start = peek();
  //   }
  //   const char *crlf = std::search(start, beginWrite(), tempCRLF, tempCRLF +
  //   2); return crlf == beginWrite() ? nullptr : crlf;
  // }

  ssize_t readFd(int fd, int *saveErrno);
  ssize_t writeFd(int fd, int *saveErrno);

 private:
  char *begin() { return buffer_.data(); }
  const char *begin() const { return buffer_.data(); }

  void makeSpace(size_t len) {
    if (writeableBytes() + prependableBytes() < len + kCheapPrepend) {
      buffer_.resize(writerIndex_ + len);
    } else {
      size_t readable = readableBytes();
      std::copy(begin() + readerIndex_, begin() + writerIndex_,
                begin() + kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
    }
  }

  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_BUFFER_H_