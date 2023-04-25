add_executable(EchoServer server.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/bin/echo)

target_link_libraries(EchoServer TinyWebNet TinyWebBase)
