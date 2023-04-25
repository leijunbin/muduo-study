#ifndef SRC_NET_INCLUDE_HTTP_HTTPRESPONSE_H_
#define SRC_NET_INCLUDE_HTTP_HTTPRESPONSE_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "../Buffer.h"

namespace TinyWeb {
namespace net {
namespace http {
class HttpResponse {
 public:
  enum HttpStatusCode {
    kUnKonwn,
    k200Ok = 200,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k404NotFound = 404,
  };

  static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
  static const std::unordered_map<HttpStatusCode, std::string> CODE_STATUS;
  static const std::unordered_map<HttpStatusCode, std::string> CODE_PATH;

  explicit HttpResponse(bool close)
      : statusCode_(kUnKonwn), closeConnection_(close) {}

  void setStatusCode(HttpStatusCode code) { statusCode_ = code; }

  void setCloseConnection(bool closeConnection) {
    closeConnection_ = closeConnection;
  }
  bool closeConnection() const { return closeConnection_; }

  void setContentType(const std::string& contentType) {
    addHeader("Content-Type", contentType);
  }

  void addHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
  }

  void setStringBody(const std::string& stringBody) {
    stringBody_ = stringBody;
  }

  void appendStringToBuffer(Buffer* output) const;
  void appendFileToBuffer(Buffer* output, off_t size) const;

 private:
  void appendRequestLintToBuffer(Buffer* output) const;
  void appendHeaderToBuffer(Buffer* output) const;
  void appendCommonMessageToBuffer(Buffer* output, off_t size) const;
  void appendRequestHeadToBuffer(Buffer* output, off_t size) const;

  std::unordered_map<std::string, std::string> headers_;
  HttpStatusCode statusCode_;

  bool closeConnection_;
  std::string stringBody_;
};
}  // namespace http
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_HTTP_HTTPRESPONSE_H_