#include "include/Timestamp.h"

#include <sys/time.h>
#include <time.h>

using namespace TinyWeb::base;

Timestamp Timestamp::now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

std::string Timestamp::toString() const {
  char buf[128] = {0};
  time_t secondSinceEpoch = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int microSecondsSinceEpoch = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  tm *tm_time = localtime(&secondSinceEpoch);
  snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d.%06d",
           tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
           tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec,
           microSecondsSinceEpoch);
  return buf;
}