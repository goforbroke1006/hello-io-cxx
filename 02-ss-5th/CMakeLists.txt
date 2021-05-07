project(02-ss-5th)

add_executable(${PROJECT_NAME} main.cpp utils.h)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} pthread)
