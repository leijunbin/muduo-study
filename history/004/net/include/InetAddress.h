#ifndef SRC_NET_INCLUDE_INETADDRESS_H_
#define SRC_NET_INCLUDE_INETADDRESS_H_

#include <arpa/inet.h>

#include <string>

namespace TinyWeb {
namespace net {
class InetAddress {
 public:
  InetAddress() = default;

  explicit InetAddress(uint16_t port, std::string ip = "0.0.0.0");

  std::string toIp() const;
  uint16_t toPort() const;
  std::string toIpPort() const;
  const sockaddr_in *getSockAddr() const { return &addr_; }

  void setSockaddr(const sockaddr_in &addr) { addr_ = addr; }

 private:
  struct sockaddr_in addr_;
};
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_INETADDRESS_H_