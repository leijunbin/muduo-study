#ifndef SRC_NET_INCLUDE_TIMER_H_
#define SRC_NET_INCLUDE_TIMER_H_

#include <atomic>

#include "../../base/include/Timestamp.h"
#include "Callbacks.h"

namespace TinyWeb {
namespace net {
class Timer {
 public:
  Timer(TimerCallback cb, base::Timestamp when, double interval)
      : timerCallback_(cb),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0),
        sequence_(++s_numCreated_) {}

  void run() const { timerCallback_(); }

  base::Timestamp expiration() const { return expiration_; }
  bool repeat() const { return repeat_; }
  int64_t sequence() const { return sequence_; }

  void restart(base::Timestamp now);

  static int64_t numCreated() { return s_numCreated_.load(); }

 private:
  const TimerCallback timerCallback_;
  base::Timestamp expiration_;
  const double interval_;
  const bool repeat_;
  const int64_t sequence_;

  static std::atomic_int64_t s_numCreated_;
};
}  // namespace net
}  // namespace TinyWeb

#endif
