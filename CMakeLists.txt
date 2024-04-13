cmake_minimum_required(VERSION 3.0)
project(new_hash_function)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)  # For SHA1 functionality

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads OpenSSL::SSL)