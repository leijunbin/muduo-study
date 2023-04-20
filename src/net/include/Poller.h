#ifndef SRC_NET_INCLUDE_POLLER_H_
#define SRC_NET_INCLUDE_POLLER_H_

#include <unordered_map>
#include <vector>

#include "../../base/include/Timestamp.h"

namespace TinyWeb {
namespace net {
class Channel;
class EventLoop;

class Poller {
 public:
  using ChannelList = std::vector<Channel *>;

  Poller(EventLoop *);
  virtual ~Poller();

  virtual base::Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
  virtual void updateChannel(Channel *channel) = 0;
  virtual void removeChannel(Channel *channel) = 0;

  bool hasChannel(Channel *channel) const;

  static Poller *newDefaultPoll(EventLoop *loop);

 protected:
  using ChannelMap = std::unordered_map<int, Channel *>;

  ChannelMap channels_;
  EventLoop *ownerLoop_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_POLLER_H_