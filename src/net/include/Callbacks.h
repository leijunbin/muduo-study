#ifndef SRC_NET_INCLUDE_CALLBACKS_H_
#define SRC_NET_INCLUDE_CALLBACKS_H_

#include <functional>
#include <memory>

using namespace TinyWeb::net;

namespace TinyWeb {
namespace net {
class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *)>;

using HighWaterMarkCallback =
    std::function<void(const TcpConnectionPtr &, size_t)>;
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_CALLBACKS_H_