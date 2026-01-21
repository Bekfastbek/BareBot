#ifndef CLIENT_H
#define CLIENT_H
#include <stdbool.h>
#include <openssl/ssl.h>



typedef struct {
    int socket_file_descriptor;
    SSL_CTX *context;
    SSL *ssl;
    bool is_connected;
} Client;

int client_connect(Client *client, const char *hostname, const char *port);
int client_send(Client *client, const char *data, unsigned int len);
int client_receive(Client *client, char *buffer, unsigned int len);
void client_cleanup(Client *client);



#endif