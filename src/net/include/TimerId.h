#ifndef SRC_NET_INCLUDE_TIMERID_H_
#define SRC_NET_INCLUDE_TIMERID_H_

#include <atomic>

namespace TinyWeb {
namespace net {
class Timer;

class TimerId {
 public:
  TimerId() : timer_(nullptr), sequence_(0) {}
  TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq) {}

  friend class TimerQueue;

 private:
  Timer* timer_;
  int64_t sequence_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_TIMERID_H_