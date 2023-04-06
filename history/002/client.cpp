#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "util.h"

int main() {
  // 创建网络套接字，返回文件描述符
  // 1:IP 地址类型,AF_INET表示IPV4,AF_INET6表示IPV6
  // 2:数据传输方式,SOCK_STREAM表示流格式,多用于TCP;SOCK_DGRAM表示数据报格式,多用于UDP
  // 3:协议,0表示自动推导协议类型，IPPROTO_TCP和IPPTOTO_UDP表示TCP和UDP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errif(sockfd == -1, "socket create error.");

  // 初始化网络地址结构
  struct sockaddr_in serv_addr;
  bzero(&serv_addr, sizeof(serv_addr));
  // IP地址类型
  serv_addr.sin_family = AF_INET;
  // 设置IP地址
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // 设置IP端口
  serv_addr.sin_port = htons(8888);

  // 连接对应IP地址端口
  // 1:套接字文件描述符
  // 2:发送目标的IP地址信息结构体指针
  // 3:发送目标的IP地址信息结构体大小
  int res = connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
  errif(res == -1, "socket connect error.");

  while (true) {
    char buf[1024];
    bzero(&buf, sizeof(buf));
    scanf("%s", buf);
    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    if (read_bytes > 0) {
      printf("message from server: %s\n", buf);
    } else if (read_bytes == 0) {
      printf("server socket disconnected!\n");
      break;
    } else if (read_bytes == -1) {
      close(sockfd);
      errif(true, "socket read error");
    }
  }
  close(sockfd);
  return 0;
}