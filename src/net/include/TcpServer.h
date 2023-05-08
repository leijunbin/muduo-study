#ifndef SRC_NET_INCLUDE_TCPSERVER_H_
#define SRC_NET_INCLUDE_TCPSERVER_H_

#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>

#include "../../base/include/noncopyable.h"
#include "Acceptor.h"
#include "Callbacks.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"

namespace TinyWeb {
namespace net {
class EventLoop;

class TcpServer : base::noncopyable {
 public:
  using ThreadInitCallback = std::function<void(EventLoop *)>;

  enum Option { kNoReusePort, kReusePort };

  TcpServer(EventLoop *loop, const InetAddress &listenAddr,
            const std::string &nameArg, Option option = kNoReusePort);
  ~TcpServer();

  void setThreadInitCallback(const ThreadInitCallback &cb) {
    threadInitCallback_ = cb;
  }
  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
  }

  void setThreadNum(int numThreads);

  void start();

  EventLoop *getLoop() const { return loop_; }

  const std::string &name() const { return name_; }

  const std::string &ipPort() const { return ipPort_; }

 private:
  void newConnection(int sockfd, const InetAddress &peerAddr);
  void removeConnection(const TcpConnectionPtr &conn);
  void removeConnectionInLoop(const TcpConnectionPtr &conn);

  using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

  EventLoop *loop_;

  const std::string ipPort_;
  const std::string name_;

  std::unique_ptr<Acceptor> acceptor_;

  std::shared_ptr<EventLoopThreadPool> threadPool_;

  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;

  ThreadInitCallback threadInitCallback_;

  std::atomic_int started_;

  int nextConnId_;
  ConnectionMap connections_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_TCPCONNECTION_H_
