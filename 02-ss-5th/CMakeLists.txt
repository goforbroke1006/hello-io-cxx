project(02-ss-5th)

add_executable(${PROJECT_NAME} main.cpp)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} pthread)

target_link_libraries(${PROJECT_NAME} prometheus-cpp-pull prometheus-cpp-core)
