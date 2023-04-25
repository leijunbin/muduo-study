#include "../include/http/HttpResponse.h"

#include "../../base/include/Logging.h"

using namespace TinyWeb::net::http;
using namespace TinyWeb::base;

const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css "},
    {".js", "text/javascript "},
    {".ico", "image/png"},
    {".mp4", "video/mp4"}};

const std::unordered_map<HttpResponse::HttpStatusCode, std::string>
    HttpResponse::CODE_STATUS = {
        {HttpResponse::k200Ok, "OK"},
        {HttpResponse::k400BadRequest, "Bad Request"},
        {HttpResponse::k301MovedPermanently, "Moved Permanently"},
        {HttpResponse::k404NotFound, "Not Found"},
        {HttpResponse::kUnKonwn, "Undefined Error"}};

const std::unordered_map<HttpResponse::HttpStatusCode, std::string>
    HttpResponse::CODE_PATH = {
        {HttpResponse::k400BadRequest, "400.html"},
        {HttpResponse::k404NotFound, "404.html"},
};

void HttpResponse::appendRequestLintToBuffer(Buffer* output) const {
  char buf[32];
  memset(buf, '\0', sizeof(buf));
  snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", statusCode_);
  output->append(buf);
  output->append(CODE_STATUS.find(statusCode_)->second);
  output->append("\r\n");
}

void HttpResponse::appendCommonMessageToBuffer(Buffer* output,
                                               off_t size) const {
  char buf[32];
  if (closeConnection_) {
    output->append("Connection: close\r\n");
  } else {
    snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", size);
    output->append(buf);
    output->append("Connection: Keep-Alive\r\n");
    output->append("Server: TinyWeb\r\n");
  }
}

void HttpResponse::appendHeaderToBuffer(Buffer* output) const {
  for (const auto& header : headers_) {
    output->append(header.first);
    output->append(": ");
    output->append(header.second);
    output->append("\r\n");
  }
  output->append("\r\n");
}

void HttpResponse::appendRequestHeadToBuffer(Buffer* output, off_t size) const {
  appendRequestLintToBuffer(output);

  appendCommonMessageToBuffer(output, size);

  appendHeaderToBuffer(output);
}

void HttpResponse::appendStringToBuffer(Buffer* output) const {
  appendRequestHeadToBuffer(output, stringBody_.size());
  output->append(stringBody_);
}

void HttpResponse::appendFileToBuffer(Buffer* output, off_t size) const {
  appendRequestHeadToBuffer(output, size);
}