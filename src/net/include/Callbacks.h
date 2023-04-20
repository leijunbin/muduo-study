#ifndef SRC_NET_INCLUDE_CALLBACKS_H_
#define SRC_NET_INCLUDE_CALLBACKS_H_

#include <functional>
#include <memory>

#include "../../base/include/Timestamp.h"

using namespace TinyWeb::net;

namespace TinyWeb {
namespace net {
class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
using MessageCallback =
    std::function<void(const TcpConnectionPtr &, Buffer *, base::Timestamp)>;

using HighWaterMarkCallback =
    std::function<void(const TcpConnectionPtr &, size_t)>;

using TimerCallback = std::function<void()>;
}  // namespace net
}  // namespace TinyWeb

#endif  // SRC_NET_INCLUDE_CALLBACKS_H_