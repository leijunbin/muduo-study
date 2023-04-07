#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

int main() {
  // 创建网络套接字，返回文件描述符
  // 1:IP地址类型,AF_INET表示IPV4,AF_INET6表示IPV6
  // 2:数据传输方式,SOCK_STREAM表示流格式,多用于TCP;SOCK_DGRAM表示数据报格式,多用于UDP
  // 3:协议,0表示自动推导协议类型，IPPROTO_TCP和IPPTOTO_UDP表示TCP和UDP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errif(sockfd == -1, "socket create error");

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
  int res = bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
  errif(res == -1, "socket bind error.");

  // 设置服务端套接字监听端口
  // 1:socket实例
  // 2:listen函数最大监听队列长度，SOMAXCONN系统建议最大值
  res = listen(sockfd, SOMAXCONN);
  errif(res == -1, "socket listen error.");

  // 创建 epfd
  int epfd = epoll_create(MAX_EVENTS);
  errif(epfd == -1, "epoll create error");

  // 添加 sockfd 到 epfd
  struct epoll_event events[MAX_EVENTS], ev;
  bzero(&events, sizeof(events));

  bzero(&ev, sizeof(ev));
  ev.data.fd = sockfd;
  ev.events = EPOLLIN;
  epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

  while (true) {
    // 取出有数据发送的文件描述符
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    errif(nfds == -1, "epoll wait error");

    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == sockfd) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        bzero(&client_addr, sizeof(client_addr));

        // 接收连接监听端口的IP信息
        // 1:套接字文件描述符
        // 2:接受连接套接字IP信息结构体指针
        // 3:接受连接套接字IP信息结构体大小指针
        int client_sockfd =
            accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
        errif(client_sockfd == -1, "socket accept error.");

        printf("new client fd %d! IP: %s Port: %d\n", client_sockfd,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        bzero(&ev, sizeof(ev));
        ev.data.fd = client_sockfd;
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
      } else if (events[i].events & EPOLLIN) {
        int client_sockfd = events[i].data.fd;
        char buf[READ_BUFFER];
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(client_sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
          printf("message from client fd %d: %s\n", client_sockfd, buf);
          write(client_sockfd, buf, sizeof(buf));
        } else if (read_bytes == 0) {
          printf("client fd %d disconnected\n", client_sockfd);
          close(client_sockfd);
          continue;
        } else if (read_bytes == -1) {
          close(client_sockfd);
          continue;
        }
      } else {
        printf("something else happened\n");
      }
    }
  }
  close(sockfd);
  return 0;
}