cmake_minimum_required(VERSION 4.1)
project(bot C)

set(CMAKE_C_STANDARD 11)

add_executable(bot src/main.c
        src/env.c
        include/env.h
        src/utils/keygen.c
        include/utils.h
        include/client.h
        src/client.c)
