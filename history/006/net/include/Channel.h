#ifndef SRC_NET_INCLUDE_CHANNEL_H_
#define SRC_NET_INCLUDE_CHANNEL_H_

#include <functional>

namespace TinyWeb {
namespace net {
class EventLoop;

class Channel {
 public:
  Channel(EventLoop *loop, int fd)
      : loop_(loop), fd_(fd), events_(0), revents_(0), inEpoll_(false){};
  ~Channel();

  void handleEvent();
  void enableReading();

  int fd() const { return fd_; }
  int events() const { return events_; }
  int revents() const { return revents_; }
  bool inEpoll() const { return inEpoll_; }

  void setInEpoll();
  void setRevents(int);
  void setCallback(std::function<void()>);

 private:
  EventLoop *loop_;
  const int fd_;
  int events_;
  int revents_;
  bool inEpoll_;
  std::function<void()> callback_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_CHANNEL_H_
