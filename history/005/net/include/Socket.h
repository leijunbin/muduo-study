#ifndef SRC_NET_INCLUDE_SOCKET_H_
#define SRC_NET_INCLUDE_SOCKET_H_

namespace TinyWeb {
namespace net {
class InetAddress;

class Socket {
 public:
  explicit Socket();
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

  int fd() const { return sockfd_; }
  void bindAddress(InetAddress *const localaddr);
  void listen();
  int accept(InetAddress *peeraddr);

  static int createNoneblockingFD();

 private:
  // const
  int sockfd_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_SOCKET_H_