add_executable(Timer timer.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/bin/timer)

target_link_libraries(Timer TinyWebNet TinyWebBase)