project (EchoServer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/bin)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-g -std=c++11")

add_subdirectory(./src/base)

add_subdirectory(./src/net)

add_executable(EchoServer ./example/server.cpp)

target_link_libraries(EchoServer TinyWebNet TinyWebBase)