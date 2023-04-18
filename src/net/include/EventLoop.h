#ifndef SRC_NET_INCLUDE_EPOLLLOOP_H_
#define SRC_NET_INCLUDE_EPOLLLOOP_H_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace TinyWeb {
namespace net {

class Poller;
class Channel;

class EventLoop {
 public:
  using Functor = std::function<void()>;

  EventLoop();
  ~EventLoop();

  void loop();
  void quit();

  void runInLoop(Functor cb);
  void queueInLoop(Functor cb);

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
  std::unique_ptr<Poller> poller_;

  int wakeupFd_;
  std::unique_ptr<Channel> wakeupChannel_;

  ChannelList activeChannels_;

  std::atomic_bool callingPendingFunctors_;
  std::vector<Functor> pendingFunctors_;
  std::mutex mutex_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EPOLLLOOP_H_