#include "include/Acceptor.h"

#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/InetAddress.h"
#include "include/Socket.h"

using namespace TinyWeb::net;

Acceptor::Acceptor(EventLoop *loop) : loop_(loop) {
  // 创建网络套接字，返回文件描述符
  sock_ = new Socket();
  // 初始化网络地址结构
  addr_ = new InetAddress(8888, "127.0.0.1");
  // 将套接字与IP地址和端口号绑定
  sock_->bindAddress(addr_);
  // 设置服务端套接字监听端口
  sock_->listen();
  acceptChannel_ = new Channel(loop_, sock_->fd());
  std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
  acceptChannel_->setCallback(cb);
  acceptChannel_->enableReading();
}

Acceptor::~Acceptor() {
  delete sock_;
  delete addr_;
  delete acceptChannel_;
}

void Acceptor::acceptConnection() {
  InetAddress *clnt_addr = new InetAddress();
  Socket *clnt_sock = new Socket(sock_->accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->fd(),
         clnt_addr->toIp().c_str(), clnt_addr->toPort());
  newConnectionCallback(clnt_sock);
  delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> cb) {
  newConnectionCallback = cb;
}