#ifndef SRC_BASE_INCLUDE_THREAD_H_
#define SRC_BASE_INCLUDE_THREAD_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "noncopyable.h"

namespace TinyWeb {
namespace base {
class Thread : noncopyable {
 public:
  using ThreadFunc = std::function<void()>;

  explicit Thread(ThreadFunc func, const std::string &name = std::string());
  ~Thread();

  void start();
  void join();

  bool started() const { return started_; }
  const std::string &name() const { return name_; }
  unsigned long getThreadId() const;

  static int numCreated() { return numCreated_; }

 private:
  bool started_ = false;
  bool joined_ = false;
  std::thread::id id_;
  std::shared_ptr<std::thread> thread_;
  ThreadFunc func_;
  std::string name_ = "";
  static std::atomic_int numCreated_;
};
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_THREAD_H_
