#ifndef SRC_BASE_INCLUDE_LOGFILE_H_
#define SRC_BASE_INCLUDE_LOGFILE_H_

#include <memory>
#include <mutex>

#include "FileUtil.h"

namespace TinyWeb {
namespace base {
class LogFile {
 public:
  LogFile(const std::string &basename, off_t rollSize, int flushInterval = 3,
          int checkEveryN = 1024);
  ~LogFile();

  void append(const char *data, int len);
  void flush();
  bool rollFile();

 private:
  static std::string getLogFileName(const std::string &basename, time_t *now);
  void appendInLock(const char *data, int len);

  const std::string basename_;
  const off_t rollSize_;
  const int flushInterval_;
  const int checkEveryN_;

  int count_;

  std::unique_ptr<std::mutex> mutex_;
  time_t startOfPeriod_;
  time_t lastRoll_;
  time_t lastFlush_;
  std::unique_ptr<FileUtil> file_;

  const static int kRollPerSeconds_ = 60 * 60 * 24;
};
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_LOGFILE_H_
