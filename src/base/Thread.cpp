#include "include/Thread.h"

#include <semaphore.h>

#include <cstring>

using namespace TinyWeb::base;

std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : id_(0), func_(std::move(func)), name_(name) {
  if (name_.empty()) {
    name_ =
        std::string("Thread") + std::to_string(static_cast<int>(numCreated_++));
  }
}

Thread::~Thread() {
  if (started_ && !joined_) {
    thread_->detach();
  }
}

void Thread::start() {
  if (started_) {
    return;
  }

  sem_t sem;
  sem_init(&sem, false, 0);

  started_ = true;
  thread_ = std::make_shared<std::thread>([&]() {
    id_ = std::this_thread::get_id();
    sem_post(&sem);
    func_();
  });

  sem_wait(&sem);
}

void Thread::join() {
  joined_ = true;
  thread_->join();
}

unsigned long Thread::getThreadId() const {
  unsigned long id;
  memcpy(&id, &id_, 8);
  return id;
}