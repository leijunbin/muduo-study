#ifndef SRC_NET_INCLUDE_ACCEPTOR_H_
#define SRC_NET_INCLUDE_ACCEPTOR_H_

#include <functional>

#include "../../base/include/noncopyable.h"

namespace TinyWeb {
namespace net {
class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor : base::noncopyable {
 public:
  using NewConnectionCallback = std::function<void(int, const InetAddress &)>;

  Acceptor(EventLoop *loop, const InetAddress &listenAddr,
           bool reuseport = true);
  ~Acceptor();

  void setNewConnectionCallback(const NewConnectionCallback &cb) {
    newConnectionCallback_ = cb;
  }

  void listen();

  bool listenning() const { return listenning_; }

 private:
  void handleRead();

  EventLoop *loop_;
  Socket *acceptSock_;
  Channel *acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listenning_ = false;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_ACCEPTOR_H_