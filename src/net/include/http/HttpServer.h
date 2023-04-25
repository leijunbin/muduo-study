#ifndef SRC_NET_INCLUDE_HTTP_HTTPSERVER_H_
#define SRC_NET_INCLUDE_HTTP_HTTPSERVER_H_

#include <functional>
#include <map>

#include "../../../base/include/Logging.h"
#include "../Buffer.h"
#include "../Callbacks.h"
#include "../TcpConnection.h"
#include "../TcpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace TinyWeb {
namespace net {
namespace http {
using namespace TinyWeb::base;

class HttpServer {
 public:
  using HttpCallback = std::function<void(const HttpRequest &, HttpResponse *)>;
  using PathMap = std::map<std::string, HttpCallback>;

  enum FileType {
    Static,
    Download,
  };

  HttpServer(EventLoop *loop, InetAddress &listenAddr, const std::string &name,
             TcpServer::Option option = TcpServer::kNoReusePort);

  EventLoop *getLoop() const { return server_.getLoop(); }

  void setHttpCallback(const HttpCallback &cb) { httpCallback_ = cb; }

  void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

  void start() {
    LOG_INFO << "HttpServer[" << server_.name() << "] starts listening on "
             << server_.ipPort();
    server_.start();
  }

  void Get(const std::string &path, HttpCallback cb);
  void Post(const std::string &path, HttpCallback cb);
  void StaticFile(const std::string &path, std::string filename);
  void DownloadFile(const std::string &path, std::string filename);
  void setStaticDir(const std::string &staticDir);

 private:
  void sendWithBuffer(const TcpConnectionPtr &conn, Buffer *buf);

  void onConnection(const TcpConnectionPtr &conn);
  void onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                 Timestamp receiveTime);
  void onRequest(const TcpConnectionPtr &, const HttpRequest &);
  bool sendFile(const std::string &filename, const TcpConnectionPtr &conn,
                HttpResponse *resp, FileType fileType);

  TcpServer server_;
  HttpCallback httpCallback_;
  PathMap getMap_;
  PathMap postMap_;
  std::map<std::string, std::string> staticFiles_;
  std::map<std::string, std::string> downloadFiles_;
  std::string staticDir_ = "./static/";
};
}  // namespace http
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_HTTP_HTTPSERVER_H_
