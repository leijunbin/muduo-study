#include <iostream>

#include "../include/EventLoop.h"
using namespace TinyWeb::net;

int main() {
  EventLoop loop;
  int num = 0;

  loop.runEvery(1, [&num]() { std::cout << ++num << "s\n"; });

  loop.runAfter(6, [&loop]() {
    std::cout << "timer finish\n";
    loop.quit();
  });

  loop.loop();
}