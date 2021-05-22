project(05-ss-epoll)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} prometheus-cpp-pull prometheus-cpp-core)
