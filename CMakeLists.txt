cmake_minimum_required(VERSION 4.1)
project(bot C)

set(CMAKE_C_STANDARD 23)
find_package(OpenSSL REQUIRED)
include_directories(include ${OPENSSL_INCLUDE_DIR})

set(SRC src/utils/env.c
        src/utils/keygen.c
        include/utils.h
        include/network.h
        src/network/client.c
)

set(TESTS tests/tests.c
          tests/test_client.c
          tests/test_env.c
          tests/test_keygen.c
        tests/test_http.c
        src/network/websocket.c
        tests/test_websocket.c
)

add_executable(run_tests ${SRC} ${TESTS})
add_executable(main src/main.c ${SRC})
target_link_libraries(run_tests OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(main OpenSSL::SSL OpenSSL::Crypto)