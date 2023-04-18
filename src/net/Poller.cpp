#include "include/Poller.h"

#include "include/Channel.h"
#include "include/EpollPoller.h"
#include "include/EventLoop.h"

using namespace TinyWeb::net;

Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {}

Poller *Poller::newDefaultPoll(EventLoop *loop) {
  return new EPollPoller(loop);
}

bool Poller::hasChannel(Channel *channel) const {
  auto it = channels_.find(channel->fd());
  return it != channels_.end() && it->second == channel;
}

Poller::~Poller() {}