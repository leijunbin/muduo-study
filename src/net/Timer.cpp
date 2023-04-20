#include "include/Timer.h"

using namespace TinyWeb::net;

std::atomic_int64_t Timer::s_numCreated_{0};

void Timer::restart(base::Timestamp now) {
  if (repeat_) {
    expiration_ = base::addTime(now, interval_);
  } else {
    expiration_ = base::Timestamp();
  }
}