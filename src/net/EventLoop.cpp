#include "include/EventLoop.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <cstring>

#include "../base/include/Logging.h"
#include "include/Channel.h"
#include "include/Poller.h"
#include "include/TimerQueue.h"

using namespace TinyWeb::net;
using namespace TinyWeb::base;

thread_local EventLoop *t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

static int createEvent() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_FATAL << "eventfd error:" << errno;
  }
  return evtfd;
}

unsigned long EventLoop::get_thread_id() {
  unsigned long id;
  memcpy(&id, &threadId_, 8);
  return id;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      callingPendingFunctors_(false),
      threadId_(std::this_thread::get_id()),
      poller_(Poller::newDefaultPoll(this)),
      wakeupFd_(createEvent()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      timerQueue_(new TimerQueue(this)) {
  if (t_loopInThisThread) {
    LOG_FATAL << "Another EventLoop exists in this thread " << get_thread_id();
  } else {
    t_loopInThisThread = this;
  }

  wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
  wakeupChannel_->enableReading();
  LOG_TRACE << "EventLoop ctor end";
}

EventLoop::~EventLoop() {
  wakeupChannel_->disableAll();
  wakeupChannel_->remove();
  ::close(wakeupFd_);
  t_loopInThisThread = nullptr;
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}

void EventLoop::loop() {
  looping_.store(true);
  quit_.store(false);

  while (!quit_.load()) {
    activeChannels_.clear();
    pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

    for (Channel *channel : activeChannels_) {
      LOG_TRACE << "EventLoop::loop get in channelHandle fd=" << channel->fd();
      channel->handleEvent(pollReturnTime_);
    }

    doPendingFunctors();
  }

  looping_.store(false);
}

void EventLoop::quit() {
  quit_.store(true);

  if (!isInLoopThread()) {
    wakeup();
  }
}

void EventLoop::runInLoop(Functor cb) {
  int inThread = isInLoopThread();
  LOG_TRACE << "EventLoop::runInLoop in loopThread:" << inThread;
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(Functor cb) {
  {
    std::lock_guard<std::mutex> lg(mutex_);
    pendingFunctors_.push_back(cb);
  }

  if (!isInLoopThread() || callingPendingFunctors_.load()) {
    wakeup();
  }
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback cb) {
  return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb) {
  Timestamp time(addTime(Timestamp::now(), delay));
  return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb) {
  Timestamp time(addTime(Timestamp::now(), interval));
  return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId) { timerQueue_->cancel(timerId); }

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = write(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instand of 8";
  }
}

void EventLoop::updateChannel(Channel *channel) {
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
  poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
  return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors() {
  LOG_TRACE << "EventLoop::doPendingFunctors callback";

  std::vector<Functor> functors;
  callingPendingFunctors_.store(true);

  {
    std::lock_guard<std::mutex> lg(mutex_);
    functors.swap(pendingFunctors_);
  }

  for (const Functor &functor : functors) {
    functor();
  }
  callingPendingFunctors_.store(false);
}

void EventLoop::abortNotInLoopThread() {
  unsigned long id;
  std::thread::id curId = std::this_thread::get_id();
  memcpy(&id, &curId, 8);
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << get_thread_id()
            << ", current thread id = " << id;
}