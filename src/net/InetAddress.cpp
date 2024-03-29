#include "include/InetAddress.h"

#include <cstring>

#include "../base/include/Logging.h"

using namespace TinyWeb::net;
using namespace TinyWeb::base;

sockaddr_in InetAddress::getLocalAddr(int sockfd) {
  sockaddr_in localaddr{0};
  socklen_t addrlen = sizeof(localaddr);
  if (::getsockname(sockfd, (sockaddr*)&localaddr, &addrlen)) {
    LOG_ERROR << "Socket::getLocalAddr";
  }
  return localaddr;
}

sockaddr_in InetAddress::getPeerAddr(int sockfd) {
  sockaddr_in peeraddr{0};
  socklen_t addrlen = sizeof(peeraddr);
  if (::getpeername(sockfd, (sockaddr*)&peeraddr, &addrlen)) {
    LOG_ERROR << "Socket::getLocalAddr";
  }
  return peeraddr;
}

InetAddress::InetAddress(uint16_t port, std::string ip) {
  // 初始化网络地址结构
  bzero(&addr_, sizeof(addr_));
  // IP地址类型
  addr_.sin_family = AF_INET;
  // 设置IP地址
  addr_.sin_port = htons(port);
  // 设置IP端口
  addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const {
  char buf[64] = {0};
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  return buf;
}

uint16_t InetAddress::toPort() const { return ::ntohs(addr_.sin_port); }

std::string InetAddress::toIpPort() const {
  char buf[64] = {0};
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  uint16_t port = ntohs(addr_.sin_port);
  size_t end = strlen(buf);
  sprintf(buf + end, ":%u", port);
  return buf;
}