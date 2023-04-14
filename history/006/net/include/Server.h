#ifndef SRC_NET_INCLUDE_SERVER_H_
#define SRC_NET_INCLUDE_SERVER_H_

#include <map>

namespace TinyWeb {
namespace net {
class EventLoop;
class Socket;
class Channel;
class Acceptor;
class Connection;
class Server {
 private:
  EventLoop *loop_;
  Acceptor *acceptor_;
  std::map<int, Connection *> Connections_;

 public:
  Server(EventLoop *);
  ~Server();

  void newConnection(Socket *sock);
  void deleteConnection(Socket *sock);
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_SERVER_H_