cmake_minimum_required(VERSION 2.6)

project(codl)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -Iinclude -fPIC")
add_compile_options(-pedantic -Wall -Wextra -Wsign-conversion -Wconversion -Wshadow)
include_directories(include/)

file(GLOB
    SRCS "src/*.c")

add_library(${PROJECT_NAME} SHARED ${SRCS})

install(FILES libcodl.so DESTINATION /lib)
install(FILES include/codl.h DESTINATION /usr/include/)
