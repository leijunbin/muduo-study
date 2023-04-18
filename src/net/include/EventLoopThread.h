#ifndef SRC_NET_INCLUDE_EVENTLOOPTHREAD_H_
#define SRC_NET_INCLUDE_EVENTLOOPTHREAD_H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

#include "../../base/include/Thread.h"

namespace TinyWeb {
namespace net {

class EventLoop;

class EventLoopThread {
 public:
  using ThreadInitCallback = std::function<void(EventLoop *)>;

  EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                  const std::string &name = std::string());
  ~EventLoopThread();

  EventLoop *startLoop();

 private:
  void threadFunc();

  EventLoop *loop_ = nullptr;
  bool exiting_ = false;
  base::Thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
  ThreadInitCallback callback_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EVENTLOOPTHREAD_H_
