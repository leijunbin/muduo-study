#ifndef SRC_NET_INCLUDE_HTTP_HTTPREQUEST_H_
#define SRC_NET_INCLUDE_HTTP_HTTPREQUEST_H_

#include <assert.h>

#include <string>
#include <unordered_map>

#include "../../../base/include/Logging.h"
#include "../../../base/include/Timestamp.h"

namespace TinyWeb {
namespace net {
namespace http {
using namespace TinyWeb::base;

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
  void setPathWithString(const std::string& path) { path_ = path; }
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

  const std::unordered_map<std::string, std::string>& post() const {
    return post_;
  }
  void parseFromUrlencoded_() {
    if (body_.size() == 0) {
      return;
    }
    std::string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;

    for (; i < n; i++) {
      char ch = body_[i];
      switch (ch) {
        case '=':
          key = body_.substr(j, i - j);
          j = i + 1;
          break;
        case '+':
          body_[i] = ' ';
          break;
        case '%':
          num = ConverHex(body_[i + 1]) * 16 + ConverHex(body_[i + 2]);
          body_[i + 2] = num % 10 + '0';
          body_[i + 1] = num / 10 + '0';
          i += 2;
          break;
        case '&':
          value = body_.substr(j, i - j);
          j = i + 1;
          post_[key] = value;
          LOG_DEBUG << key << " = " << value;
          break;
        default:
          break;
      }
    }
    assert(j <= i);
    if (post_.count(key) == 0 && j < i) {
      value = body_.substr(j, i - j);
      post_[key] = value;
      LOG_DEBUG << key << " = " << value;
    }
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
  int ConverHex(char ch) {
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'A' + 10;
    return ch;
  }

  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  std::string body_;
  TinyWeb::base::Timestamp receiveTime_;
  std::unordered_map<std::string, std::string> headers_;
  std::unordered_map<std::string, std::string> post_;
};
}  // namespace http
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_HTTP_HTTPREQUEST_H_
