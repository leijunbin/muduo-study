#include "include/Logging.h"

using namespace TinyWeb::base;

namespace ThreadInfo {
__thread char errnobuf[512];
}  // namespace ThreadInfo

const char* TinyWeb::base::getErrnoMsg(int savedErrno) {
  return strerror_r(savedErrno, ThreadInfo::errnobuf,
                    sizeof(ThreadInfo::errnobuf));
}

const char* getLevelName[Logger::LogLevel::LEVEL_COUNT]{
    "TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL ",
};

Logger::LogLevel initLogLevel() { return Logger::INFO; }

Logger::LogLevel TinyWeb::base::g_logLevel = initLogLevel();

static void defaultOutput(const char* data, int len) {
  fwrite(data, len, sizeof(char), stdout);
}

static void defaultFlush() { fflush(stdout); }

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Impl::Impl(LogLevel level, int savedErrno, const char* file, int line)
    : time_(Timestamp::now()),
      stream_(),
      level_(level),
      line_(line),
      basename_(file) {
  formatTime();
  stream_ << GeneralTemplate(getLevelName[level], 6);
  if (savedErrno != 0) {
    stream_ << getErrnoMsg(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::formatTime() {
  Timestamp now = Timestamp::now();
  int microseconds = static_cast<int>(now.microSecondsSinceEpoch() %
                                      Timestamp::kMicroSecondsPerSecond);

  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "%06d ", microseconds);
  stream_ << GeneralTemplate(now.toString().c_str(), 26) << ' ';
}

void Logger::Impl::finish() {
  stream_ << " - " << GeneralTemplate(basename_.data_, basename_.size_) << ':'
          << line_ << '\n';
}

Logger::Logger(const char* file, int line) : impl_(INFO, 0, file, line) {}

Logger::Logger(const char* file, int line, Logger::LogLevel level)
    : impl_(level, 0, file, line) {}

Logger::Logger(const char* file, int line, Logger::LogLevel level,
               const char* func)
    : impl_(level, 0, file, line) {
  impl_.stream_ << func << ' ';
}

Logger::~Logger() {
  impl_.finish();
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
  if (impl_.level_ == FATAL) {
    g_flush();
    abort();
  }
}

void Logger::setLogLevel(Logger::LogLevel level) { g_logLevel = level; }

void Logger::setOutput(OutputFunc out) { g_output = out; }

void Logger::setFlush(FlushFunc flush) { g_flush = flush; }