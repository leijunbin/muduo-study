#ifndef SRC_NET_INCLUDE_HTTP_HTTPREQUEST_H_
#define SRC_NET_INCLUDE_HTTP_HTTPREQUEST_H_

#include <string>
#include <unordered_map>

#include "../../../base/include/Timestamp.h"

namespace TinyWeb {
namespace net {
namespace http {
class HttpRequest {
 public:
  enum Method { kInvalid, kGet, kPost, kHead, kPut, kDelete };
  enum Version { kUnKnown, kHttp10, kHttp11 };

  HttpRequest() : method_(kInvalid), version_(kUnKnown) {}

  void setVersion(Version v) { version_ = v; }
  Version version() const { return version_; }

  bool setMethod(const char* start, const char* end) {
    std::string m(start, end);
    if (m == "GET") {
      method_ = kGet;
    } else if (m == "POST") {
      method_ = kPost;
    } else if (m == "HEAD") {
      method_ = kHead;
    } else if (m == "PUT") {
      method_ = kPut;
    } else if (m == "DELETE") {
      method_ = kDelete;
    } else {
      method_ = kInvalid;
    }
    return method_ != kInvalid;
  }
  Method method() const { return method_; }
  const char* methodString() const {
    const char* result = "UNKNOWN";
    switch ((method_)) {
      case kGet:
        result = "GET";
        break;
      case kPost:
        result = "POST";
        break;
      case kHead:
        result = "HEAD";
        break;
      case kPut:
        result = "PUT";
        break;
      case kDelete:
        result = "DELETE";
        break;
      default:
        break;
    }
    return result;
  }

  void setPath(const char* start, const char* end) { path_.assign(start, end); }
  const std::string& path() const { return path_; }

  void setQuery(const char* start, const char* end) {
    query_.assign(start, end);
  }
  const std::string& query() const { return query_; }

  void setBody(const std::string& body) { body_ = body; }
  const std::string& body() const { return body_; }

  void setReceiveTime(TinyWeb::base::Timestamp t) { receiveTime_ = t; }
  TinyWeb::base::Timestamp receiveTime() const { return receiveTime_; }

  void addHeader(const char* start, const char* colon, const char* end) {
    std::string field(start, colon);
    ++colon;
    while (colon < end && isspace(*colon)) {
      ++colon;
    }
    std::string value(colon, end);
    while (!value.empty() && isspace(value[value.size() - 1])) {
      value.resize(value.size() - 1);
    }
    headers_[field] = value;
  }
  std::string getHeader(const std::string& field) const {
    std::string result;
    auto it = headers_.find(field);
    if (it != headers_.end()) {
      result = it->second;
    }
    return result;
  }
  const std::unordered_map<std::string, std::string>& headers() const {
    return headers_;
  }

  void swap(HttpRequest& rhs) {
    std::swap(method_, rhs.method_);
    std::swap(version_, rhs.version_);
    path_.swap(rhs.path_);
    query_.swap(rhs.query_);
    std::swap(receiveTime_, rhs.receiveTime_);
    headers_.swap(rhs.headers_);
  }

 private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  std::string body_;
  TinyWeb::base::Timestamp receiveTime_;
  std::unordered_map<std::string, std::string> headers_;
};
}  // namespace http
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_HTTP_HTTPREQUEST_H_
