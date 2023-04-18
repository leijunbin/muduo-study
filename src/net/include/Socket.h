#ifndef SRC_NET_INCLUDE_SOCKET_H_
#define SRC_NET_INCLUDE_SOCKET_H_

namespace TinyWeb {
namespace net {
class InetAddress;

class Socket {
 public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

  int fd() const { return sockfd_; }
  void bindAddress(const InetAddress &localaddr);
  void listen();
  int accept(InetAddress *peeraddr);

  void shutdownWrite();

  void setTcpNoDelay(bool on);
  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setKeepAlive(bool on);

  static int createNoneblockingFD();
  static int getSocketError(int sockfd);
  static bool isSelfConnect(int sockfd);

 private:
  const int sockfd_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_SOCKET_H_