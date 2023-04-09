#ifndef SRC_NET_INCLUDE_CONNECTION_H_
#define SRC_NET_INCLUDE_CONNECTION_H_
#include <functional>

namespace TinyWeb {
namespace net {
class EventLoop;
class Socket;
class Channel;
class Connection {
 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *channel_;
  std::function<void(Socket *)> deleteConnectionCallback;

 public:
  Connection(EventLoop *loop, Socket *sock);
  ~Connection();

  void echo(int sockfd);
  void setDeleteConnectionCallback(std::function<void(Socket *)>);
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_CONNECTION_H_