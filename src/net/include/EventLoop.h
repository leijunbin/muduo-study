#ifndef SRC_NET_INCLUDE_EPOLLLOOP_H_
#define SRC_NET_INCLUDE_EPOLLLOOP_H_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "../../base/include/Timestamp.h"
#include "Callbacks.h"
#include "TimerId.h"

namespace TinyWeb {
namespace net {

class Poller;
class Channel;
class TimerQueue;

class EventLoop {
 public:
  using Functor = std::function<void()>;

  EventLoop();
  ~EventLoop();

  void loop();
  void quit();

  base::Timestamp pollReturnTime() const { return pollReturnTime_; }

  void runInLoop(Functor cb);
  void queueInLoop(Functor cb);

  TimerId runAt(base::Timestamp time, TimerCallback cb);
  TimerId runAfter(double delay, TimerCallback cb);
  TimerId runEvery(double interval, TimerCallback cb);
  void cancel(TimerId timerId);

  void wakeup();

  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);
  bool hasChannel(Channel *channel);

  bool isInLoopThread() { return threadId_ == std::this_thread::get_id(); }

  unsigned long get_thread_id();

 private:
  void handleRead();
  void doPendingFunctors();

  using ChannelList = std::vector<Channel *>;

  std::atomic_bool looping_;
  std::atomic_bool quit_;
  const std::thread::id threadId_;
  base::Timestamp pollReturnTime_;
  std::unique_ptr<Poller> poller_;

  int wakeupFd_;
  std::unique_ptr<Channel> wakeupChannel_;

  ChannelList activeChannels_;
  std::unique_ptr<TimerQueue> timerQueue_;

  std::atomic_bool callingPendingFunctors_;
  std::vector<Functor> pendingFunctors_;
  std::mutex mutex_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EPOLLLOOP_H_