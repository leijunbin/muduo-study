#ifndef SRC_NET_INCLUDE_EPOLLLOOP_H_
#define SRC_NET_INCLUDE_EPOLLLOOP_H_

namespace TinyWeb {
namespace net {
class Epoll;
class Channel;
class EventLoop {
 private:
  Epoll *ep_;
  bool quit_;

 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel *);
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EPOLLLOOP_H_