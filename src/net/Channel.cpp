#include "include/Channel.h"

#include "include/EventLoop.h"

using namespace TinyWeb::net;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false){};

void Channel::handleEvent(base::Timestamp receiveTime) {
  std::shared_ptr<void> guard;
  if (tied_) {
    guard = tie_.lock();
    if (guard) {
      handleEventWithGuard(receiveTime);
    }
  } else {
    handleEventWithGuard(receiveTime);
  }
}

void Channel::tie(const std::shared_ptr<void> &obj) {
  tie_ = obj;
  tied_ = true;
}

void Channel::remove() { loop_->removeChannel(this); }

void Channel::update() { loop_->updateChannel(this); }

void Channel::handleEventWithGuard(base::Timestamp receiveTime) {
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    if (closeCallback_) {
      // log
      closeCallback_();
    }
  }

  if (revents_ & EPOLLERR) {
    if (errorCallback_) {
      // log
      errorCallback_();
    }
  }

  if (revents_ & (EPOLLIN | EPOLLPRI)) {
    if (readCallback_) {
      readCallback_(receiveTime);
    }
  }

  if (revents_ & EPOLLOUT) {
    if (writeCallback_) {
      writeCallback_();
    }
  }

  // log
}