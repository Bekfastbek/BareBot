#ifndef TESTS_H
#define TESTS_H



extern struct report_test {
    bool env;
    bool keygen;
    bool client;
    bool http;
    bool websocket;
} report;

void test_env();
void test_keygen();
void test_client();
void test_http();
void test_websocket();



#endif