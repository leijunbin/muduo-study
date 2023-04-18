#include "include/Acceptor.h"

#include <unistd.h>

#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/InetAddress.h"
#include "include/Socket.h"

using namespace TinyWeb::net;

static int createNoneblocking() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0) {
    // log
  }
  return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr,
                   bool reuseport)
    : loop_(loop) {
  // 创建网络套接字，返回文件描述符
  acceptSock_ = new Socket(createNoneblocking());

  acceptChannel_ = new Channel(loop, acceptSock_->fd());

  acceptSock_->setReuseAddr(true);
  acceptSock_->setReusePort(reuseport);

  acceptSock_->bindAddress(listenAddr);

  acceptChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
  acceptChannel_->disableAll();
  acceptChannel_->remove();
}

void Acceptor::handleRead() {
  InetAddress peeraddr;
  int connfd = acceptSock_->accept(&peeraddr);
  // log
  if (connfd >= 0) {
    if (newConnectionCallback_) {
      newConnectionCallback_(connfd, peeraddr);
    } else {
      ::close(connfd);
    }
  } else {
    // log
    if (errno == EMFILE) {
      // log
    }
  }
}

void Acceptor::listen() {
  // log
  listenning_ = true;
  acceptSock_->listen();
  acceptChannel_->enableReading();
}