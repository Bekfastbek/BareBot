#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "../include/client.h"



int client_connect(DiscordClient *client, const char *hostname, const char *port) {

    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    printf("[INFO] Resolving hostname: %s...\n", hostname);
    if ((status =  getaddrinfo(hostname, port, &hints, &res)) != 0) {
        fprintf(stderr, "[ERROR] getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    for (p = res; p!= NULL; p = p -> ai_next) {
        void *addr;
        const char *ipver;

        if(p -> ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p -> ai_addr;
            addr = &(ipv4 -> sin_addr);
            ipver = "IPv4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p -> ai_addr;
            addr = &(ipv6 -> sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(p -> ai_family, addr, ipstr, sizeof ipstr);
        printf("[INFO] Attempting to connect to %s: %s\n", ipver, ipstr);

        client -> socket_file_descriptor = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol);
        if (client -> socket_file_descriptor == -1) {
            perror("[WARN] socket creation failed, trying with different ipaddr");
            continue;
        }

        if (connect(client -> socket_file_descriptor, p -> ai_addr, p -> ai_addrlen) == -1) {
            close(client -> socket_file_descriptor);
            perror("[WARN] Connect failed, trying with different ipaddr");
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "[ERROR] Failed to connect after multiple different ipaddr.");
        return -1;
    }

    printf("Connect to %s\n", hostname);
    client -> is_connected = 1;
    return 0;
}



void client_cleanup(DiscordClient *client) {
    if (client -> socket_file_descriptor > 0) {
        close(client -> socket_file_descriptor);
        client -> socket_file_descriptor = 0;
    }
    client -> is_connected = 0;
    printf("[INFO] Socket closed.\n");
}