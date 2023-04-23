project (EchoServer)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-g -std=c++11")

add_subdirectory(./src/base)

add_subdirectory(./src/net)

add_subdirectory(./example)

add_subdirectory(./src/base/test)

add_subdirectory(./src/net/test)
