#include <unistd.h>

#include <cstring>

#include "net/include/Epoll.h"
#include "net/include/InetAddress.h"
#include "net/include/Socket.h"
#include "util.h"

#define READ_BUFFER 1024

using namespace TinyWeb::net;

int main() {
  // 创建网络套接字，返回文件描述符
  Socket *serv_sock = new Socket();

  // 初始化网络地址结构
  InetAddress *serv_addr = new InetAddress(8888, "127.0.0.1");

  // 将套接字与IP地址和端口号绑定
  serv_sock->bindAddress(*serv_addr);
  delete serv_addr;

  // 设置服务端套接字监听端口
  serv_sock->listen();

  // 创建 epfd
  Epoll *ep = new Epoll();

  // 添加 sockfd 到 epfd
  ep->addFd(serv_sock->fd(), EPOLLIN);

  while (true) {
    // 取出有数据发送的文件描述符
    std::vector<epoll_event> events = ep->poll(-1);
    int nfds = events.size();
    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == serv_sock->fd()) {
        InetAddress *client_addr = new InetAddress();
        // 接收连接监听端口的IP信息
        Socket *client_sock = new Socket(serv_sock->accept(client_addr));
        printf("new client fd %d! IP: %s Port: %d\n", client_sock->fd(),
               client_addr->toIp().c_str(), client_addr->toPort());

        ep->addFd(client_sock->fd(), EPOLLIN);
        delete client_addr;
        // delete client_sock;
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
  delete serv_sock;
  return 0;
}