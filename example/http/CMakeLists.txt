add_executable(HttpServer server.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/bin/http)

target_link_libraries(HttpServer TinyWebNet TinyWebBase TinyWebNetHttp)