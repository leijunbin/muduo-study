#include "include/EpollPoller.h"

#include <unistd.h>

#include <cstring>

#include "include/Channel.h"

using namespace TinyWeb::net;

constexpr int kNew = -1;
constexpr int kAdded = 1;
constexpr int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  if (epollfd_ < 0) {
    // TODO: log
  }
}

EPollPoller::~EPollPoller() { ::close(epollfd_); }

void EPollPoller::poll(ChannelList *activeChannels) {
  int numEvent = ::epoll_wait(epollfd_, events_.data(),
                              static_cast<int>(events_.size()), -1);
  if (numEvent > 0) {
    fillActiveChannels(numEvent, activeChannels);
    if (static_cast<size_t>(numEvent) == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvent == 0) {
    // TODO: log
  } else {
    // TODO: log
  }
}

void EPollPoller::updateChannel(Channel *channel) {
  const int index = channel->index();
  if (index == kNew || index == kDeleted) {
    if (index == kNew) {
      int fd = channel->fd();
      channels_[fd] = channel;
    }
    channel->set_index(kAdded);
    update(EPOLL_CTL_ADD, channel);
  } else {
    int fd = channel->fd();
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->set_index(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void EPollPoller::removeChannel(Channel *channel) {
  int fd = channel->fd();
  channels_.erase(fd);

  int index = channel->index();
  if (index == kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents,
                                     ChannelList *activeChannels) const {
  for (int i = 0; i < numEvents; i++) {
    Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
    channel->set_revents(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void EPollPoller::update(int operation, Channel *channel) {
  epoll_event event;
  memset(&event, 0, sizeof(event));
  int fd = channel->fd();
  event.data.ptr = channel;
  event.events = channel->events();

  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      // log
    } else {
      // log
    }
  }
}
