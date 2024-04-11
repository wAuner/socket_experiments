cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(server_client)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(server src/server.cpp)

add_executable(client src/client.cpp)