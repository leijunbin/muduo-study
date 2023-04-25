#ifndef SRC_NET_INCLUDE_TCPCONNECTION_H_
#define SRC_NET_INCLUDE_TCPCONNECTION_H_

#include <atomic>
#include <memory>
#include <string>

#include "Buffer.h"
#include "Callbacks.h"
#include "InetAddress.h"

namespace TinyWeb {
namespace net {
class EventLoop;
class Socket;
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop *loop, const std::string &nameArg, int sockfd,
                const InetAddress &localAddr, const InetAddress &peerAddr);
  ~TcpConnection();

  EventLoop *getLoop() const { return loop_; }
  const std::string &name() const { return name_; }
  const InetAddress &localAddress() const { return localAddr_; }
  const InetAddress &peerAddress() const { return peerAddr_; }

  bool connected() const { return state_.load() == kConnected; }

  void send(const std::string &buf);
  void send(const void *data, size_t len);

  void shutdown();

  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    writeCompleteCallback_ = cb;
  }
  void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }
  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb,
                                size_t highWaterMark) {
    highWaterMarkCallback_ = cb;
    highWaterMark_ = highWaterMark;
  }

  void connectEstablished();
  void connectDestroyed();

 private:
  enum StateE { kDisconnected, kConnecting, kConnected, kDisConnecting };

  void setState(StateE state) { state_.store(state); }

  void handleRead(base::Timestamp receiveTime);
  void handleWrite();
  void handleClose();
  void handleError();

  void sendInLoop(const void *data, size_t len);
  void shutdownInLoop();

  EventLoop *loop_;
  const std::string name_;
  std::atomic_int state_;
  bool reading_;

  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;

  const InetAddress localAddr_;
  const InetAddress peerAddr_;

  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  CloseCallback closeCallback_;

  HighWaterMarkCallback highWaterMarkCallback_;
  size_t highWaterMark_;

  Buffer inputBuffer_;
  Buffer outputBuffer_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_TCPCONNECTION_H_
