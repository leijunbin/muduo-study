#include "../include/http/HttpContext.h"

#include "../../base/include/Logging.h"

using namespace TinyWeb::net::http;
using namespace TinyWeb::base;

bool HttpContext::processRequestLine(const char* begin, const char* end) {
  LOG_DEBUG << "HttpContext::processRequestLint running";

  bool succeed = false;
  const char* start = begin;
  const char* space = std::find(start, end, ' ');

  if (space != end && request_.setMethod(start, space)) {
    start = space + 1;
    space = std::find(start, end, ' ');
    if (space != end) {
      const char* question = std::find(start, space, '?');
      if (question != space) {
        request_.setPath(start, question);
        request_.setQuery(question, space);
      } else {
        request_.setPath(start, space);
      }
      start = space + 1;
      succeed = (end - start == 8 && std::equal(start, end - 1, "HTTP/1."));
      if (succeed) {
        if (*(end - 1) == '1') {
          request_.setVersion(HttpRequest::kHttp11);
        } else if (*(end - 1) == '0') {
          request_.setVersion(HttpRequest::kHttp10);
        } else {
          succeed = false;
        }
      }
    }
  }
  return succeed;
}

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime) {
  LOG_DEBUG << "HttpContext::parseRequest running";

  bool ok = false;
  bool hasMore = true;

  while (hasMore) {
    if (state_ == kExpectRequestLine) {
      LOG_DEBUG << "HttpContext RequestLine";
      const char* crlf = buf->findCRLF();
      if (crlf) {
        ok = processRequestLine(buf->peek(), crlf);
        if (ok) {
          request_.setReceiveTime(receiveTime);
          buf->retrieveUntil(crlf + 2);
          state_ = kExpectHeaders;
        } else {
          hasMore = false;
        }
      } else {
        hasMore = false;
      }
    } else if (state_ == kExpectHeaders) {
      LOG_DEBUG << "HttpContext RequestHeaders";
      const char* crlf = buf->findCRLF();
      if (crlf) {
        const char* colon = std::find(buf->peek(), crlf, ':');
        if (colon != crlf) {
          request_.addHeader(buf->peek(), colon, crlf);
        } else {
          if (request_.method() == HttpRequest::kPost) {
            state_ = kExpectBody;
          } else {
            state_ = kGotAll;
            hasMore = false;
          }
        }
        buf->retrieveUntil(crlf + 2);
      } else {
        hasMore = false;
      }
    } else if (state_ == kExpectBody) {
      LOG_DEBUG << "HttpContext Body";
      request_.setBody(buf->retrieveAsString(buf->readableBytes()));
      if (request_.headers().find("Content-Type")->second ==
          "application/x-www-form-urlencoded") {
        request_.parseFromUrlencoded_();
      }
      state_ = kGotAll;
      hasMore = false;
    }
  }

  LOG_DEBUG << "HttpContext ParseRequest end";
  return ok;
}