#ifndef SRC_BASE_INCLUDE_FILEUTIL_H_
#define SRC_BASE_INCLUDE_FILEUTIL_H_

#include <stdio.h>

#include <string>

namespace TinyWeb {
namespace base {
class FileUtil {
 public:
  explicit FileUtil(std::string& fileName);
  ~FileUtil();

  void append(const char* data, size_t len);

  void flush();

  off_t writtenBytes() const { return writtenBytes_; }

 private:
  size_t write(const char* data, size_t len);

  FILE* fp_;
  char buffer_[64 * 1024];
  off_t writtenBytes_;
};
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_FILEUTIL_H_