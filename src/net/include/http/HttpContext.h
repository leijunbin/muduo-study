#ifndef SRC_NET_INCLUDE_HTTP_HTTPCONTEXT_H_
#define SRC_NET_INCLUDE_HTTP_HTTPCONTEXT_H_

#include "../Buffer.h"
#include "HttpRequest.h"

namespace TinyWeb {
namespace net {
namespace http {
class HttpContext {
 public:
  enum HttpRequestParseState {
    kExpectRequestLine,
    kExpectHeaders,
    kExpectBody,
    kGotAll,
  };

  HttpContext() : state_(kExpectRequestLine) {}

  bool parseRequest(Buffer* buf, TinyWeb::base::Timestamp receiveTime);

  bool gotAll() const { return state_ == kGotAll; }

  void reset() {
    state_ = kExpectRequestLine;
    HttpRequest dummy;
    request_.swap(dummy);
  }

  const HttpRequest& request() const { return request_; }

  HttpRequest& request() { return request_; }

 private:
  bool processRequestLine(const char* begin, const char* end);

  HttpRequestParseState state_;
  HttpRequest request_;
};
}  // namespace http
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_HTTP_HTTPCONTEXT_H_
