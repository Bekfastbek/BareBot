cmake_minimum_required(VERSION 4.1)
project(bot C)

set(CMAKE_C_STANDARD 11)
find_package(OpenSSL REQUIRED)
include_directories(include ${OPENSSL_INCLUDE_DIR})

set(SRC src/env.c
        include/env.h
        src/utils/keygen.c
        include/utils.h
        include/client.h
        src/client.c
)

set(TESTS tests/tests.c
          tests/test_client.c
          tests/test_env.c
          tests/test_keygen.c
)

add_executable(run_tests ${SRC} ${TESTS})
add_executable(main src/main.c ${SRC})
target_link_libraries(run_tests OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(main OpenSSL::SSL OpenSSL::Crypto)