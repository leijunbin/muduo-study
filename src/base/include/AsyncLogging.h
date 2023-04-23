#ifndef SRC_BASE_INCLUDE_ASYNCLOGGING_H_
#define SRC_BASE_INCLUDE_ASYNCLOGGING_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

#include "FixedBuffer.h"
#include "Thread.h"

namespace TinyWeb {
namespace base {
class AsyncLogging {
 public:
  AsyncLogging(const std::string& basename, off_t rollSize,
               int flushInterval = 3);
  ~AsyncLogging() {
    if (running_) {
      stop();
    }
  }

  void append(const char* logline, int len);

  void start() {
    running_ = true;
    thread_.start();
  }

  void stop() {
    running_ = false;
    cond_.notify_one();
    thread_.join();
  }

 private:
  using Buffer = FixedBuffer<kLargeBuffer>;
  using BufferVector = std::vector<std::unique_ptr<Buffer>>;
  using BufferPtr = BufferVector::value_type;

  void threadFunc();

  const int flushInterval_;
  std::atomic_bool running_;
  const std::string basename_;
  const off_t rollSize_;
  Thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;

  BufferPtr currentBuffer_;
  BufferPtr nextBuffer_;
  BufferVector buffers_;
};
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_ASYNCLOGGING_H_
