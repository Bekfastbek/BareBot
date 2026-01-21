#ifndef TESTS_H
#define TESTS_H
#include <stdbool.h>



extern struct Report {
    bool env;
    bool keygen;
    bool client;
} report;



void test_env();
void test_keygen();
void test_client();



#endif