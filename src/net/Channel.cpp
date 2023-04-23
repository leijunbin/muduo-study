#include "include/Channel.h"

#include "../base/include/Logging.h"
#include "include/EventLoop.h"

using namespace TinyWeb::net;
using namespace TinyWeb::base;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false){};

void Channel::handleEvent(base::Timestamp receiveTime) {
  LOG_DEBUG << "Channel::handleEvent for fd=" << fd_ << " and tie is "
            << (int)tied_;
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
  LOG_TRACE << "channel handleEvent revents:" << revents_;

  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    if (closeCallback_) {
      LOG_DEBUG << "Channel::handleEventWithGuard for closeCallback";
      closeCallback_();
    }
  }

  if (revents_ & EPOLLERR) {
    if (errorCallback_) {
      LOG_DEBUG << "Channel::handleEventWithGuard for errorCallback";
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

  LOG_TRACE << "Channel::handleEventLWithGuard end";
}