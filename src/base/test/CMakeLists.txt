add_executable(AsyncLoggingTest AsyncLoggingTest.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/bin/asyncLoggingTest)

target_link_libraries(AsyncLoggingTest TinyWebBase)