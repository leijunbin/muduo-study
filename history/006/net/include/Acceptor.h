#ifndef SRC_NET_INCLUDE_ACCEPTOR_H_
#define SRC_NET_INCLUDE_ACCEPTOR_H_

#include <functional>

namespace TinyWeb {
namespace net {
class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor {
 private:
  EventLoop *loop_;
  Socket *sock_;
  InetAddress *addr_;
  Channel *acceptChannel_;
  std::function<void(Socket *)> newConnectionCallback;

 public:
  Acceptor(EventLoop *loop);
  ~Acceptor();
  void acceptConnection();
  void setNewConnectionCallback(std::function<void(Socket *)>);
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_ACCEPTOR_H_