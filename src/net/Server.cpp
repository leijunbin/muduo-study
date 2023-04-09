#include "include/Server.h"

#include <unistd.h>

#include <cstring>

#include "include/Acceptor.h"
#include "include/Channel.h"
#include "include/Connection.h"
#include "include/EventLoop.h"
#include "include/InetAddress.h"
#include "include/Socket.h"

using namespace TinyWeb::net;

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : loop_(loop) {
  acceptor_ = new Acceptor(loop);
  std::function<void(Socket *)> cb =
      std::bind(&Server::newConnection, this, std::placeholders::_1);
  acceptor_->setNewConnectionCallback(cb);
}

Server::~Server() { delete acceptor_; }

void Server::newConnection(Socket *sock) {
  Connection *conn = new Connection(loop_, sock);
  std::function<void(Socket *)> cb =
      std::bind(&Server::deleteConnection, this, std::placeholders::_1);
  conn->setDeleteConnectionCallback(cb);
  Connections_[sock->fd()] = conn;
}

void Server::deleteConnection(Socket *sock) {
  Connection *conn = Connections_[sock->fd()];
  Connections_.erase(sock->fd());
  delete conn;
}