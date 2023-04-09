#include "include/EventLoop.h"

#include <vector>

#include "include/Channel.h"
#include "include/Epoll.h"

using namespace TinyWeb::net;

EventLoop::EventLoop() : ep_(nullptr), quit_(false) { ep_ = new Epoll(); }

EventLoop::~EventLoop() { delete ep_; }

void EventLoop::loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = ep_->poll();
    for (auto it = chs.begin(); it != chs.end(); ++it) {
      (*it)->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *ch) { ep_->updateChannel(ch); }