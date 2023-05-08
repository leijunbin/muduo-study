#ifndef SRC_BASE_INCLUDE_NONCOPYABLE_H_
#define SRC_BASE_INCLUDE_NONCOPYABLE_H_

namespace TinyWeb {
namespace base {
class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;

  noncopyable(noncopyable &&) = default;
  noncopyable &operator=(noncopyable &&) = default;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};
}  // namespace base
}  // namespace TinyWeb

#endif  // SRC_BASE_INCLUDE_NONCOPYABLE_H_
