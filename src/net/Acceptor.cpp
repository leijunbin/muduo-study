#include "include/Acceptor.h"

#include <unistd.h>

#include "../base/include/Logging.h"
#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/InetAddress.h"
#include "include/Socket.h"

using namespace TinyWeb::net;
using namespace TinyWeb::base;

static int createNoneblocking() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0) {
    LOG_FATAL << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__
              << " listen socket create err:" << errno;
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
  LOG_DEBUG << "Acceptor::handleRead and peerAddr:"
            << peeraddr.toIpPort().c_str() << " and connfd=" << connfd;
  if (connfd >= 0) {
    if (newConnectionCallback_) {
      newConnectionCallback_(connfd, peeraddr);
    } else {
      ::close(connfd);
    }
  } else {
    LOG_ERROR << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__
              << " accept err:" << errno;
    if (errno == EMFILE) {
      LOG_ERROR << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__
                << "  sockfd reached limit";
    }
  }
}

void Acceptor::listen() {
  LOG_DEBUG << "Acceptor::listen begin to listen";
  listenning_ = true;
  acceptSock_->listen();
  acceptChannel_->enableReading();
}