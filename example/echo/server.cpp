#include <functional>

#include "../../src/base/include/Logging.h"
#include "../../src/base/include/Timestamp.h"
#include "../../src/net/include/Buffer.h"
#include "../../src/net/include/Callbacks.h"
#include "../../src/net/include/EventLoop.h"
#include "../../src/net/include/TcpConnection.h"
#include "../../src/net/include/TcpServer.h"
using namespace TinyWeb::net;
using namespace TinyWeb::base;

class EchoServer {
 public:
  EchoServer(EventLoop *loop, const InetAddress &addr, const std::string &name)
      : server_(loop, addr, name), loop_(loop) {
    // 注册回调函数
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, std::placeholders::_1));

    server_.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));

    // 设置合适的subloop线程数量
    server_.setThreadNum(3);
  }
  void start() { server_.start(); }

 private:
  void onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
      LOG_INFO << "Connection UP : " << conn->peerAddress().toIpPort().c_str();
    } else {
      LOG_INFO << "Connection DOWN : "
               << conn->peerAddress().toIpPort().c_str();
    }
  }

  void onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                 TinyWeb::base::Timestamp time) {
    std::string msg = buf->retrieveAsString(buf->readableBytes());
    LOG_INFO << conn->name().c_str() << " echo " << static_cast<int>(msg.size())
             << " bytes,content:" << msg.c_str() << ";in "
             << time.toString().c_str();
    conn->send(msg);
  }

  EventLoop *loop_;
  TcpServer server_;
};

int main() {
  // Logger::setLogLevel(Logger::TRACE);
  EventLoop loop;
  InetAddress addr(8086);
  EchoServer server(&loop, addr, "EchoServer");
  server.start();
  loop.loop();
  return 0;
}