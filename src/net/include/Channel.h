#ifndef SRC_NET_INCLUDE_CHANNEL_H_
#define SRC_NET_INCLUDE_CHANNEL_H_

#include <sys/epoll.h>

#include <functional>
#include <memory>

namespace TinyWeb {
namespace net {
class EventLoop;

class Channel {
 public:
  using EventCallback = std::function<void()>;
  // using ReadEventCallback =

  Channel(EventLoop *loop, int fd);

  void handleEvent();

  void setReadCallback(EventCallback cb) { readCallback_ = cb; }
  void setWriteCallback(EventCallback cb) { writeCallback_ = cb; }
  void setCloseCallback(EventCallback cb) { closeCallback_ = cb; }
  void setErrorCallback(EventCallback cb) { errorCallback_ = cb; }

  void tie(const std::shared_ptr<void> &);

  int fd() const { return fd_; }
  int events() const { return events_; }
  void set_revents(int revt) { revents_ = revt; }

  void enableReading() {
    events_ |= kReadEvent;
    update();
  }
  void disableReading() {
    events_ &= ~kReadEvent;
    update();
  }

  void enabelWriting() {
    events_ |= kWriteEvent;
    update();
  }
  void disableWriting() {
    events_ &= ~kWriteEvent;
    update();
  }

  void disableAll() {
    events_ = kNoneEvent;
    update();
  }

  bool isNoneEvent() const { return events_ == kNoneEvent; }
  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & kReadEvent; }

  int index() const { return index_; }
  void set_index(int idx) { index_ = idx; }

  EventLoop *ownerLoop() { return loop_; }

  void remove();

 private:
  void update();
  void handleEventWithGuard();

  static const int kNoneEvent = 0;
  static const int kReadEvent = EPOLLIN | EPOLLPRI;
  static const int kWriteEvent = EPOLLOUT;

  EventLoop *loop_;
  const int fd_;
  int events_;
  int revents_;
  int index_;

  std::weak_ptr<void> tie_;
  bool tied_;

  EventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_CHANNEL_H_
