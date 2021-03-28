cmake_minimum_required(VERSION 3.10)

project(codl)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-std=c89 -pedantic -Wall -Wextra -Wsign-conversion -Wconversion -Wshadow)

if (WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE)
endif()

include_directories(include/)

file(GLOB
    SRCS "src/*.c")

add_library(${PROJECT_NAME} SHARED ${SRCS})

install(TARGETS ${PROJECT_NAME} DESTINATION /lib)
install(FILES include/codl.h DESTINATION /usr/include/)
