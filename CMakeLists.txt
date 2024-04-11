cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 23)

project(server_client)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(server src/server.cpp)

add_executable(client src/client.cpp)