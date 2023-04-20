#ifndef SRC_BASE_INCLUDE_TIMESTAMP_H_
#define SRC_BASE_INCLUDE_TIMESTAMP_H_

#include <string>

namespace TinyWeb {
namespace base {
class Timestamp {
 public:
  Timestamp() : microSecondsSinceEpoch_(0) {}
  explicit Timestamp(int64_t microSecondsSinceEpoch)
      : microSecondsSinceEpoch_(microSecondsSinceEpoch) {}

  static Timestamp now();

  std::string toString() const;

  int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

  bool valid() const { return microSecondsSinceEpoch_ > 0; }

  static const int kMicroSecondsPerSecond = 1000 * 1000;

 private:
  int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs) {
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
  int64_t delta =
      static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_TIMESTAMP_H_