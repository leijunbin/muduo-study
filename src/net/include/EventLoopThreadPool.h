#ifndef SRC_NET_INCLUDE_EVENTLOOPTHREADPOOL_H_
#define SRC_NET_INCLUDE_EVENTLOOPTHREADPOOL_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../../base/include/noncopyable.h"
#include "EventLoopThread.h"

namespace TinyWeb {
namespace net {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : base::noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop *)>;

  EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg);

  void setThreadNum(int numThreads) { numThread_ = numThreads; }

  void start(const ThreadInitCallback &cb = ThreadInitCallback());

  EventLoop *getNextLoop();

  std::vector<EventLoop *> getAllLoops();

  bool started() const { return started_; }
  const std::string &name() const { return name_; }

 private:
  EventLoop *baseLoop_;
  std::string name_;
  bool started_ = false;
  int numThread_ = 0;
  int next_ = 0;
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop *> loops_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EVENTLOOPTHREADPOOL_H_