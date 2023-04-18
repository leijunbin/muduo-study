# socket 编程基础

## 主机字节序和网络字节序

+ 主机字节序：现代 PC 大多采用小端字节序，因此小段字节序又称之为主机字节序。
+ 网络字节序：网络发送数据总是使用大端字节序，因此大端字节序也称之为网络字节序。

## 主机字节序与网络字节序转换

Linux 提供了如下4个函数来完成主机字节序和网络字节序之间的转换：

```cpp
#include <netinet/in.h>
unsigned long int htonl(unsigned long int hostlong);
unsigned short int htons(unsigned short int hostshort);
unsigned long int ntohl(unsigned long int netlong);
unsigned short int ntohs(unsigned short int netshort);
```

htonl 表示“host to network long”，即将长整型（32 bit）的主机字节序转化为网络字节序数据。这4个函数中，长整型函数通常用来转换 IP 地址，短整型函数用来转换端口号。

## 通用 socket 地址

socket 网络编程接口中表示 socket 地址的是结构体 sockaddr，其定义如下：

```cpp
#include <bits/socket.h>
struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
}
```

+ sa_family：是地址类型变量，地址族类型通常与协议族类型对应，常见的协议族和对应地址族如下表所示：

| 协议族   | 地址族   | 描述              |
| -------- | -------- | ----------------- |
| PF_UNIX  | AF_UNIX  | UNIX 本地域协议族 |
| PF_INET  | AF_INET  | TCP/IPv4 协议族   |
| PF_INET6 | AF_INET6 | TCP/IPv6 协议族   |

宏 PF_* 和 AF_* 都定义在 bit/socket.h 头文件中，且后者与前者有完全相同的值，所以二者通常混用。

+ sa_data 用于存放 socket 地址值，不同协议族的地址值有不同的含义和长度，如下表所示：

| 协议族   | 地址值含义和长度                                                          |
| -------- | ------------------------------------------------------------------------- |
| PF_UNIX  | 文件的路径名，长度可达108字节                                             |
| PF_INET  | 16 bit 端口号和 32 bit IPv4 地址，共6字节                                 |
| PF_INET6 | 16 bit 端口号，32 bit 流标识，128 bit IPv6 地址，32 bit 范围 ID，共26字节 |

14字节的 sa_data 根本无法完全容纳多数协议族的地址值，因此，Linux 定义了下面的这个新的通用 socket 地址结构体：

```cpp
#include <bits/socket.h>
struct sockaddr_storage {
	sa_family_t sa_family;
	unsigned long int __ss_align;
	char __ss_padding[128 - siseof(__ss_align)];
}
```

## 专用 socket 地址

通用 socket 地址结构体在设置和获取 IP 地址和端口号时需要繁琐的位操作，因此 Linux 为各个协议提供了专用的 socket 地址结构体。

+ UNIX 本地域协议族使用的专用 socket 地址结构体：

```cpp
#include <sys/un.h>
struct socketaddr_un {
	sa_family_t sin_family; // 地址族：AF_UNIX
	char sun_path[108];     // 文件路径名
}
```

+ IPv4 协议族专用 socket 地址结构体：

```cpp
struct sockaddr_in {
	sa_family_t sin_family;      // 地址族：AF_INET
	u_int16_t sin_port;          // 端口号，要用网络字节序表示
	struct in_addr sin_addr;     // IPv4 地址结构体
}
struct in_addr {
	u_int32_t s_addr;            // IPv4 地址，要用网络字节序表示
}
```

+ IPv6 协议族专用 socket 地址结构体：

```cpp
struct sockaddr_in6 {
	sa_family_t sin6_family;      // 地址族：AF_INET6
	u_int16_t sin6_port;          // 端口号，要用网络字节序表示
	u_int32_t sin6_flowinfo;      // 流信息，应设置为0
	struct in6_addr sin6_addr;    // IPv4 地址结构体
	u_int32_t sin6_scope_id;      // scope ID
}
struct in6_addr {
	unsigned char sa_addr[16];    // IPv6 地址，要用网络字节序表示
}
```

所有专用 socket 地址（以及 sockaddr_storage）类型变量在实际使用时都需要转化为通用 socket 地址类型 sockaddr（强制转换即可），因为所有 socket 编程接口使用的地址参数的类型都是 sockaddr。

## IP 地址转换函数

```cpp
#include <arpa/inet.h>
in_addr_t inet_addr(const char* strptr);
int inet_aton(const char* cp, strcut in_addr* inp);
char* inet_ntoa(struct in_addr in);
```

+ inet_addr 函数将用点分的十进制字符串表示的 IPv4 地址转化为用网络字节序整数表示的 IPv4 地址，失败时返回 INADDR_NONE。
+ inet_aton 函数完成和 inet_addr 同样的功能，但是将转化结果存储与参数 inp 指向的地址结构中。成功时返回1，失败则返回0。
+ inet_ntoa 函数将用网络字节序整数表示的 IPv4 地址转化为用点分十进制字符串表示的 IPv4 地址。（注：函数内部用一个静态变量存储转化结果，函数的返回值指向该静态内存，因此 inet_ntoa 是不可重入的。）

```cpp
// inet_ntoa 不可重入实例
char* szValue1 = inet_ntoa("1.2.3.4");
char* szValue2 = inet_ntoa("10.194.71.60");
printf("address 1: %s\n",szValue1);
printf("address 2: %s\n",szValue2);
// 代码运行结果:
// address1: 10.194.71.60
// address2: 10.194.71.60
```

```cpp
#include <arpa/inet.h>
int inet_pton(int af, const char* src, void* dst);
const char* inet_ntop(int af, const void* src, char* dst, socklen_t cnt);
```

+ inet_pton 函数将用字符串表示的 IP 地址 src（用点分十进制字符串表示的 IPv4 地址或用十六进制字符串表示的 IPv6 地址）转换成为用网络字节序整数表示的 IP 地址，并把转换结果存储于 dst 指向的内存中。其中，af 参数指定地址族，可以是 AF_INET 或者 AF_INET6。inet_pton 成功时返回1，失败则返回0并设置 errno。
+ inet_ntop 函数进行相反的转换，前三个参数于 inet_pton 的参数相同，最后一个参数 cnt 指定目标存储单元的大小。inet_ntop成功时返回目标存储单元的地址，失败则返回 NULL 并设置errno。下面两个宏能够帮助指定大小（分别用于 IPv4 和 IPv6）：

```cpp
#include <netinet/in.h>
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN6 48
```

## 创建 socket

socket 也是可读、可写、可控制、可关闭的文件描述符。

```cpp
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```

+ domain 参数告诉系统使用哪个底层协议族。支持 PF_INET、PF_INET6、PF_UNIX。
+ type 参数指定服务类型。主要由面向连接的 SOCK_STREAM 服务（流服务）和面向数据报 SOCK_UGRAM（数据报）服务，分别对应 TCP 和 UDP 协议。（补充：SOCK_NONBLOCK ——非阻塞 socket，SOCK_CLOEXEC —— fork 后不能够继承父进程的 socket）
+ protocol 参数是在前两个参数构成的协议集合下，在选择一个具体的协议，不过这个值通常是唯一的，所以一般设置为0，表示使用默认协议。
+ socket 系统调用成功返回一个 socket 文件描述符，失败则返回-1并设置 errno。

## 命名 socket

```cpp
#include <sys/types.h>
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr* my_addr, socklen_t addrlen);
```

bind 将 my_addr 所指的 socket 地址分配给未命名的 sockfd 文件描述符，addrlen 参数指出该 socket 地址的长度。

bind 成功时返回0，失败则返回-1并设置 errno。其中两种常见的 errno 是 EACCES 和 EADDRINUSE，含义如下：

+ EACCES：被绑定的地址是受保护的地址，仅超级用户访问，比如将 socket 绑定到知名服务端口（1-1023）上时，bind 将返回此错误。
+ EADDRINUSE：被绑定的地址正在使用中。比如将 socket 绑定到一个处于 TIME_WAIT 状态的 socket 地址。

## 监听 socket

```cpp
#include <sys/socket.h>
int listen(int sockfd, int backlog);
```

+ sockfd 参数指定被监听的 socket。
+ backlog 参数提示内核监听队列的最大长度。监听队列的长度如果超过 backlog，服务器将不受理新的客户连接，客户端也将收到 ECONNREFUSED 错误信息。
+ listen 成功时返回1，失败则返回-1并设置 errno。

## 接受连接

```cpp
#include <sys/types.h>
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr* addr, socklen_t *addrlen);
```

+ sockfd 参数是执行过 listen 系统调用的监听 socket。
+ addr 参数用来获取被接受连接的远端 socket 地址。
+ addrlen 参数用来获取被接受连接的远端 socket 地址的长度。
+ accept 成功返回一个新的连接 socket，该 socket 唯一的标识了被接受的这个连接，服务器可通过读写该 socket 来与被接受的连接对应的客户端通信。
+ accept 失败时返回-1并设置 errno。

## 发起连接

```cpp
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
```

+ sockfd 参数是由 socket 系统调用返回一个 socket。
+ serv_addr 参数是服务器监听的 socket 地址。
+ addrlen 参数则指定这个地址长度。
+ connect 成功时返回0，失败则返回-1并设置errno。其中两种常见的 errno 是 ECONNREFUSED 和 ETIMEDOUT，它们含义如下：
  + ECONNREFUSED：目标端口不存在，连接被拒绝。
  + ETIMEDOUT：连接超时。

## 关闭连接

```cpp
#include <unistd.h>
int close(int fd);
```

+ fd 参数是待关闭的 socket。
+ 关闭连接时只能将 socket 上的读和写同时关闭。
+ 其并非总是立即关闭一个连接，而是将 fd 的引用计数减1。只有当fd 的引用计数为0时，才真正关闭连接。

如果想要立刻终止连接，可以使用 shutdown 系统调用：

```cpp
#include <sys/socket.h>
int shutdown(int sockfd, int howto);
```

+ sockfd 参数时待关闭的 socket。
+ howto 参数决定了 shutdown 的行为，可以取下表的某个值：

| 可选值    | 含义                                                                                                                                                                    |
| --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| SHUT_RD   | 关闭 sockfd 上读的这一半。应用程序不能再针对 socket 文件描述符执行读操作，并且该 socket 接收缓冲区中的数据都被丢弃。                                                    |
| SHUT_WR   | 关闭 sockfd 上写的这一半。sockfd 的发送缓冲区中的数据会在真正关闭连接之前全部发送出去，应用程序不可再对该 socket 文件描述符执行写操作。这种情况下，连接处于半关闭状态。 |
| SHUT_RDWR | 同时关闭 sockfd 上的读和写。                                                                                                                                            |

+ shutdown 成功时返回0，失败则返回-1并设置 errno。

## TCP 数据读写

文件读写操作 read 和 write 同样适用于 socket。

```cpp
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t nbytes);
ssize_t write(int fd, void *buf, size_t nbytes);
```

+ fd 参数表示初始化的文件描述符。
+ buf 参数表示缓冲区位置指针。
+ nbytes 参数表示缓冲区大小。
+ read 和 write 成功时返回读写长度，失败时返回-1并设置 errno，读取到 EOF 时返回0。
