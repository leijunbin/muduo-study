#include "include/Channel.h"

#include <sys/epoll.h>

#include "include/EventLoop.h"

using namespace TinyWeb::net;

Channel::~Channel() {}

void Channel::enableReading() {
  events_ = EPOLLIN;
  loop_->updateChannel(this);
}

void Channel::setRevents(int ev) { revents_ = ev; }

void Channel::setInEpoll() { inEpoll_ = true; }

void Channel::handleEvent() { callback_(); }

void Channel::setCallback(std::function<void()> cb) { callback_ = cb; }