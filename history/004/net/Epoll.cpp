#include "include/Epoll.h"

#include <unistd.h>

#include <cstring>

#define MAX_EVENTS 1000

using namespace TinyWeb::net;

Epoll::Epoll() : epfd(-1), events(nullptr) {
  // 创建 epfd
  epfd = epoll_create(MAX_EVENTS);
  // TODO: log
  events = new epoll_event[MAX_EVENTS];
  bzero(events, sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epfd != -1) {
    ::close(epfd);
    epfd = -1;
  }
  delete[] events;
}

void Epoll::addFd(int fd, uint32_t op) {
  // 添加 sockfd 到 epfd
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));

  ev.data.fd = fd;
  ev.events = op;

  if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev)) {
    // TODO: log
  }
}

std::vector<epoll_event> Epoll::poll(int timeout) {
  std::vector<epoll_event> activeEvents;
  // 取出有数据发送的文件描述符
  int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
  // TODO: log
  for (int i = 0; i < nfds; ++i) {
    activeEvents.push_back(events[i]);
  }
  return activeEvents;
}