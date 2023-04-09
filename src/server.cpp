#include "net/include/Server.h"

#include "net/include/EventLoop.h"

using namespace TinyWeb::net;

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->loop();
  return 0;
}