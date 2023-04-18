#include "include/EventLoopThread.h"

#include "include/EventLoop.h"

using namespace TinyWeb::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                 const std::string &name)
    : thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      callback_(cb) {}

EventLoopThread::~EventLoopThread() {
  exiting_ = true;
  if (loop_ != nullptr) {
    loop_->quit();
    thread_.join();
  }
}

EventLoop *EventLoopThread::startLoop() {
  thread_.start();
  {
    std::unique_lock<std::mutex> ul(mutex_);
    cond_.wait(ul, [this]() { return loop_ != nullptr; });
  }
  return loop_;
}

void EventLoopThread::threadFunc() {
  EventLoop loop;

  if (callback_) {
    callback_(&loop);
  }

  {
    std::unique_lock<std::mutex> ul(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }

  loop.loop();

  std::unique_lock<std::mutex> ul(mutex_);
  loop_ = nullptr;
}