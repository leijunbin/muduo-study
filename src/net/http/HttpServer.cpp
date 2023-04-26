#include "../include/http/HttpServer.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace TinyWeb::net::http;

namespace detail {
void defaultHttpCallback(const HttpRequest &, HttpResponse *resp) {
  resp->setStatusCode(HttpResponse::k404NotFound);
  resp->setCloseConnection(true);
}
}  // namespace detail

HttpServer::HttpServer(EventLoop *loop, InetAddress &listenAddr,
                       const std::string &name, TcpServer::Option option)
    : server_(loop, listenAddr, name, option),
      httpCallback_(detail::defaultHttpCallback) {
  server_.setConnectionCallback(
      std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
  server_.setMessageCallback(
      std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::sendWithBuffer(const TcpConnectionPtr &conn, Buffer *buf) {
  std::string message = buf->retrieveAsString(buf->readableBytes());
  conn->send(message);
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
  LOG_DEBUG << "HttpServer::onConnection get connection";
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                           Timestamp receiveTime) {
  std::unique_ptr<HttpContext> context(new HttpContext);

  if (!context->parseRequest(buf, receiveTime)) {
    LOG_INFO << "parseRequest failed!";
    HttpResponse response(true);
    response.setStatusCode(HttpResponse::k400BadRequest);
    sendFile(HttpResponse::CODE_PATH.find(HttpResponse::k400BadRequest)->second,
             conn, &response, Static);
    conn->shutdown();
  }

  if (context->gotAll()) {
    LOG_INFO << "parseRequest success!";
    onRequest(conn, context->request());
    context->reset();
  }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn,
                           const HttpRequest &req) {
  const std::string &path = req.path();

  const std::string &connection = req.getHeader("Connection");
  bool close =
      connection == "close" ||
      (req.version() == HttpRequest::kHttp10 && connection != "Keep-Alive");
  HttpResponse response(close);

  if (req.method() == HttpRequest::kGet &&
      staticFiles_.find(path) != staticFiles_.end()) {
    response.setStatusCode(HttpResponse::k200Ok);
    sendFile(staticFiles_[path], conn, &response, FileType::Static);
    return;
  }

  if (req.method() == HttpRequest::kGet &&
      downloadFiles_.find(path) != downloadFiles_.end()) {
    response.setStatusCode(HttpResponse::k200Ok);
    sendFile(downloadFiles_[path], conn, &response, FileType::Download);
    return;
  }

  LOG_TRACE << "Headers " << req.methodString() << " " << req.path();
  const std::unordered_map<std::string, std::string> &headers = req.headers();
  for (const auto &header : headers) {
    LOG_TRACE << header.first << ": " << header.second;
  }

  HttpCallback httpCallback = httpCallback_;
  switch (req.method()) {
    case HttpRequest::kGet:
      if (getMap_.count(path)) httpCallback = getMap_[path];
      break;
    case HttpRequest::kPost:
      if (postMap_.count(path)) httpCallback = postMap_[path];
      break;
    default:
      break;
  }
  httpCallback(req, &response);

  if (response.statusCode() == HttpResponse::k200Ok ||
      response.statusCode() == HttpResponse::k302Found) {
    Buffer buf;
    response.appendStringToBuffer(&buf);
    sendWithBuffer(conn, &buf);
    if (response.closeConnection()) {
      conn->shutdown();
    }
  } else if (response.statusCode() == HttpResponse::k404NotFound) {
    sendFile(HttpResponse::CODE_PATH.find(HttpResponse::k404NotFound)->second,
             conn, &response, Static);
  }
}

void HttpServer::Get(const std::string &path, HttpCallback cb) {
  getMap_[path] = std::move(cb);
}

void HttpServer::Post(const std::string &path, HttpCallback cb) {
  postMap_[path] = std::move(cb);
}

void HttpServer::StaticFile(const std::string &path, std::string filename) {
  staticFiles_[path] = std::move(filename);
}

void HttpServer::DownloadFile(const std::string &path, std::string filename) {
  downloadFiles_[path] = std::move(filename);
}

bool HttpServer::sendFile(const std::string &filename,
                          const TcpConnectionPtr &conn, HttpResponse *resp,
                          FileType fileType) {
  std::string::size_type idx = filename.find_last_of('.');
  if (idx == std::string::npos) {
    resp->setContentType("text/plain");
  }
  std::string typeName = filename.substr(idx);
  resp->setContentType(HttpResponse::SUFFIX_TYPE.find(typeName)->second);
  if (fileType == FileType::Download) {
    char value[300];
    snprintf(value, sizeof(value), R"(attachment;filename="%s")",
             filename.c_str());
    resp->addHeader("Content-Disposition", value);
  }

  std::string fullfilename = staticDir_ + filename;
  struct stat file;
  int fd = open(fullfilename.c_str(), O_RDONLY | O_CLOEXEC);
  ::fstat(fd, &file);
  if (fd < 0) {
    conn->send("open " + filename + " failed");
    LOG_ERROR << "open file " << filename << " error";
    return false;
  }

  Buffer output;
  resp->appendFileToBuffer(&output, file.st_size);
  sendWithBuffer(conn, &output);

  char buf[1024]{0};
  ssize_t n = 0;
  while (1) {
    n = read(fd, buf, sizeof(buf));
    if (n == 0) break;
    conn->send(buf, n);
  }
  conn->shutdown();
  close(fd);
  return true;
}

void HttpServer::setStaticDir(const std::string &staticDir) {
  staticDir_ = staticDir;
  if (staticDir.back() != '/') staticDir_.push_back('/');
}