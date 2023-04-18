#ifndef SRC_NET_INCLUDE_EPOLLPOLLER_H_
#define SRC_NET_INCLUDE_EPOLLPOLLER_H_

#include <sys/epoll.h>

#include <vector>

#include "Poller.h"

namespace TinyWeb {
namespace net {
class Channel;

class EPollPoller : public Poller {
 public:
  EPollPoller(EventLoop *);
  ~EPollPoller();

  void poll(ChannelList *activeChannels) override;
  void updateChannel(Channel *channel) override;
  void removeChannel(Channel *channel) override;

 private:
  void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
  void update(int operation, Channel *channel);

  using EventList = std::vector<epoll_event>;
  static const int kInitEventListSize = 16;

  int epollfd_;
  EventList events_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_EPOLL_H_