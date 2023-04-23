#include "include/EpollPoller.h"

#include <unistd.h>

#include <cstring>

#include "../base/include/Logging.h"
#include "include/Channel.h"

using namespace TinyWeb::net;
using namespace TinyWeb::base;

constexpr int kNew = -1;
constexpr int kAdded = 1;
constexpr int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  if (epollfd_ < 0) {
    LOG_FATAL << "epoll_create error:" << errno;
  }
}

EPollPoller::~EPollPoller() { ::close(epollfd_); }

TinyWeb::base::Timestamp EPollPoller::poll(int timeoutMs,
                                           ChannelList *activeChannels) {
  LOG_TRACE << "func=" << __FUNCTION__
            << " => fd total count:" << channels_.size();
  int numEvents = ::epoll_wait(epollfd_, events_.data(),
                               static_cast<int>(events_.size()), timeoutMs);
  int saveErrno = errno;
  TinyWeb::base::Timestamp now(TinyWeb::base::Timestamp::now());

  if (numEvents > 0) {
    LOG_TRACE << numEvents << " events happened";
    fillActiveChannels(numEvents, activeChannels);
    if (static_cast<size_t>(numEvents) == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents == 0) {
    LOG_TRACE << __FUNCTION__ << " timeout";
  } else {
    if (saveErrno != EINTR) {
      errno = saveErrno;
      LOG_ERROR << "EPollPoller::poll() err";
    }
  }
  return now;
}

void EPollPoller::updateChannel(Channel *channel) {
  const int index = channel->index();
  LOG_TRACE << "EPollPoller::updateChannel fd=" << channel->fd()
            << " events=" << channel->events() << " index=" << index;
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
  LOG_TRACE << "func=" << __FUNCTION__ << ",fd=" << channel->fd();
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
  LOG_TRACE << "poll::epoll_wait => EPollPoller::fillActiveChannels";
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
      LOG_ERROR << "epoll_ctl del error:" << errno;
    } else {
      LOG_ERROR << "epoll_ctl add/mod error:" << errno;
    }
  }
}
