#ifndef SRC_NET_INCLUDE_EPOLL_H_
#define SRC_NET_INCLUDE_EPOLL_H_

#include <sys/epoll.h>

#include <vector>

namespace TinyWeb {
namespace net {
class Channel;

class Epoll {
 private:
  int epfd;
  struct epoll_event *events;

 public:
  Epoll();
  ~Epoll();

  void addFd(int fd, uint32_t op);
  void updateChannel(Channel *);
  std::vector<Channel *> poll(int timeout = -1);
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EPOLL_H_