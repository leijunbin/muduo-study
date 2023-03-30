#include <arpa/inet.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>

int main() {
  // 创建网络套接字，返回文件描述符
  // 1:IP地址类型,AF_INET表示IPV4,AF_INET6表示IPV6
  // 2:数据传输方式,SOCK_STREAM表示流格式,多用于TCP;SOCK_DGRAM表示数据报格式,多用于UDP
  // 3:协议,0表示自动推导协议类型，IPPROTO_TCP和IPPTOTO_UDP表示TCP和UDP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // 初始化网络地址结构
  struct sockaddr_in serv_addr;
  bzero(&serv_addr, sizeof(serv_addr));
  // IP地址类型
  serv_addr.sin_family = AF_INET;
  // 设置IP地址
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // 设置IP端口
  serv_addr.sin_port = htons(8888);

  // 将套接字与IP地址和端口号绑定
  // 1:套接字文件描述符
  // 2:套接字绑定地址和端口结构体的指针
  // 3:地址结构大小
  bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

  // 设置服务端套接字监听端口
  // 1:socket实例
  // 2:listen函数最大监听队列长度，SOMAXCONN系统建议最大值
  listen(sockfd, SOMAXCONN);

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  bzero(&client_addr, sizeof(client_addr));

  // 接收连接监听端口的IP信息
  // 1:套接字文件描述符
  // 2:接受连接套接字IP信息结构体指针
  // 3:接受连接套接字IP信息结构体大小指针
  int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);

  printf("new client fd %d! IP: %s Port: %d\n", client_sockfd,
         inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
  return 0;
}