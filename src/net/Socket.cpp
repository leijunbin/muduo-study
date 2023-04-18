#include "include/Socket.h"

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

#include "include/InetAddress.h"

using namespace TinyWeb::net;

int Socket::createNoneblockingFD() {
  // 创建网络套接字，返回文件描述符
  // 1:IP地址类型,AF_INET表示IPV4,AF_INET6表示IPV6
  // 2:数据传输方式,SOCK_STREAM表示流格式,多用于TCP;SOCK_DGRAM表示数据报格式,多用于UDP（补充:SOCK_NONBLOCK非阻塞socket,SOCK_CLOEXEC
  // exec()时自动清除socket）
  // 3:协议,0表示自动推导协议类型，IPPROTO_TCP和IPPTOTO_UDP表示TCP和UDP
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0) {
    // log
  }
  return sockfd;
}

int Socket::getSocketError(int sockfd) {
  int optval;
  socklen_t optlen = sizeof(optval);
  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

bool Socket::isSelfConnect(int sockfd) {
  sockaddr_in localaddr = InetAddress::getLocalAddr(sockfd);
  sockaddr_in peeraddr = InetAddress::getPeerAddr(sockfd);
  return localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr &&
         localaddr.sin_port == peeraddr.sin_port;
}

Socket::~Socket() { ::close(sockfd_); }

void Socket::bindAddress(const InetAddress &localaddr) {
  // 将套接字与IP地址和端口号绑定
  // 1:套接字文件描述符
  // 2:套接字绑定地址和端口结构体的指针
  // 3:地址结构大小
  if (0 != ::bind(sockfd_, (sockaddr *)localaddr.getSockAddr(),
                  sizeof(sockaddr_in))) {
    // TODO: log
  }
}

void Socket::listen() {
  // 设置服务端套接字监听端口
  // 1:socket实例
  // 2:listen函数最大监听队列长度，SOMAXCONN系统建议最大值
  if (0 != ::listen(sockfd_, 1024)) {
    // TODO: log
  }
}

int Socket::accept(InetAddress *peeraddr) {
  // 初始化网络地址结构
  sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  socklen_t len = sizeof(addr);
  // 接收连接监听端口的IP信息
  // 1:套接字文件描述符
  // 2:接受连接套接字IP信息结构体指针
  // 3:接受连接套接字IP信息结构体大小指针
  int connfd =
      ::accept4(sockfd_, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
  // TODO: log
  if (connfd >= 0) {
    peeraddr->setSockaddr(addr);
  }
  return connfd;
}

void Socket::shutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR) < 0) {
    // log
  }
}

void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}