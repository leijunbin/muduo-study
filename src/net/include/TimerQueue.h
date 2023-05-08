#ifndef SRC_NET_INCLUDE_TIMERQUEUE_H_
#define SRC_NET_INCLUDE_TIMERQUEUE_H_

#include <atomic>
#include <set>
#include <vector>

#include "../../base/include/Timestamp.h"
#include "../../base/include/noncopyable.h"
#include "Callbacks.h"
#include "Channel.h"

namespace TinyWeb {
namespace net {
class EventLoop;
class Timer;
class TimerId;

class TimerQueue : base::noncopyable {
 public:
  explicit TimerQueue(EventLoop *loop);
  ~TimerQueue();

  TimerId addTimer(TimerCallback cb, base::Timestamp when, double interval);

  void cancel(TimerId timerId);

 private:
  using Entry = std::pair<base::Timestamp, Timer *>;
  using TimerList = std::set<Entry>;
  using ActiveTimer = std::pair<Timer *, int64_t>;
  using ActiveTimerSet = std::set<ActiveTimer>;

  void addTimerInLoop(Timer *timer);
  void cancelInLoop(TimerId timerId);

  void handleRead();

  std::vector<Entry> getExpired(base::Timestamp now);
  void reset(const std::vector<Entry> &expired, base::Timestamp now);

  bool insert(Timer *timer);

  EventLoop *loop_;
  const int timerfd_;
  Channel timerfdChannel_;

  TimerList timers_;

  ActiveTimerSet activeTimers_;
  std::atomic_bool callingExpiredTimers_;
  ActiveTimerSet cancelingTimers_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_TIMERQUEUE_H_