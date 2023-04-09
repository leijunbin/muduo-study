#include "include/Connection.h"

#include <unistd.h>

#include <cstring>
#include <iostream>

#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/Socket.h"

#define READ_BUFFER 1024

using namespace TinyWeb::net;

Connection::Connection(EventLoop *loop, Socket *sock)
    : loop_(loop), sock_(sock), channel_(nullptr) {
  channel_ = new Channel(loop_, sock_->fd());
  std::function<void()> cb = std::bind(&Connection::echo, this, sock_->fd());
  channel_->setCallback(cb);
  channel_->enableReading();
}

Connection::~Connection() {
  delete channel_;
  delete sock_;
}

void Connection::echo(int sockfd) {
  char buf[READ_BUFFER];
  bzero(&buf, sizeof(buf));
  ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
  if (read_bytes > 0) {
    printf("message from client fd %d: %s\n", sockfd, buf);
    write(sockfd, buf, sizeof(buf));
  } else if (read_bytes == 0) {
    printf("client fd %d disconnected\n", sockfd);
    deleteConnectionCallback(sock_);
  } else if (read_bytes == -1) {
    deleteConnectionCallback(sock_);
  }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> cb) {
  deleteConnectionCallback = cb;
}