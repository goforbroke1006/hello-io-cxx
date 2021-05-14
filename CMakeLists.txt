cmake_minimum_required(VERSION 2.8)
project(server_samples)

add_compile_options(
        -Wall -Wextra -pedantic
        -Werror
        -Werror=unused-variable
        -Werror=return-type
)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(./client)

add_subdirectory(./01-ss-1th)
add_subdirectory(./02-ss-5th)
add_subdirectory(./03-ss-select)
add_subdirectory(./04-ss-poll)
add_subdirectory(./05-ss-epoll)
