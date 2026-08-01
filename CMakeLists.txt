cmake_minimum_required(VERSION 3.15)
project(infra_roadmap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(infra src/main.cpp)

if(MINGW)
    target_compile_options(infra PRIVATE -Wall -Wextra -pedantic)
    target_link_options(infra PRIVATE -static-libgcc -static-libstdc++)
endif()

