#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../../include/utils.h"
#include "../../include/network.h"



int client_connect(Client *client, const char *hostname, const char *port) {

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
            const auto ipv4 = (struct sockaddr_in *)p -> ai_addr;
            addr = &(ipv4 -> sin_addr);
            ipver = "IPv4";
        } else {
            const auto ipv6 = (struct sockaddr_in6 *)p -> ai_addr;
            addr = &(ipv6 -> sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(p -> ai_family, addr, ipstr, sizeof ipstr);
        printf("[INFO] Attempting to connect to %s: %s\n", ipver, ipstr);

        client -> socket_file_descriptor = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol);
        if (client -> socket_file_descriptor == -1) {
            fprintf(stderr, "[ERROR] Socket creation failed.\n");
            return -1;
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

    client -> context = SSL_CTX_new(TLS_client_method());
    if (!client -> context) {
        fprintf(stderr, "[ERROR] SSL Context failed.\n");
        close(client -> socket_file_descriptor);
        return -1;
    }
    client -> ssl = SSL_new(client -> context);
    SSL_set_fd(client -> ssl, client -> socket_file_descriptor);
    SSL_set_tlsext_host_name(client -> ssl, hostname);

    printf("[INFO] Performing handshake...\n");
    if (SSL_connect(client -> ssl) != 1) {
        fprintf(stderr, "[ERROR] Handshake failed.\n");
        ERR_print_errors_fp(stderr);
        SSL_free(client -> ssl);
        SSL_CTX_free(client -> context);
        close(client -> socket_file_descriptor);
        return -1;
    }

    printf("Connected to %s\n", SSL_get_cipher(client -> ssl));
    client -> is_connected = true;
    return 0;
}



int client_send(const Client *client, const char *data, const int len) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    int sent = 0;
    int bytes_left = len;

    while (sent < len) {
        const int n = SSL_write(client -> ssl, data + sent, bytes_left);

        if (n <= 0) {
            const int err = SSL_get_error(client -> ssl, n);
            if (err == SSL_ERROR_ZERO_RETURN) {
                break;
            }
            fprintf(stderr, "[ERROR] SSL_write failed with error: %d\n", err);
            return -1;
        }
        sent += n;
        bytes_left -= n;
    }

    return sent;
}



int client_receive(const Client *client, char *buf, const int len) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    const int n = SSL_read(client -> ssl, buf, len -1);

    if (n <= 0) {
        const int err = SSL_get_error(client -> ssl, n);
        if (err == SSL_ERROR_ZERO_RETURN) {
            return 0;
        }
        return -1;
    }

    buf[n] = '\0';
    return n;
}



int client_post(const Client *client, const char *path, const char *json_body) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char req_buf[4096];
    const unsigned int body_len = (unsigned int)strlen(json_body);

    const int len = snprintf(req_buf, sizeof(req_buf),
        "POST %s HTTP/1.1\r\n"
        "Host: discord.com\r\n"
        "Authorization: Bot %s\r\n"
        "User-Agent: DiscordBot(BareBot, 1.0)\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %u\r\n"
        "\r\n"
        "%s",
        path,
        config.discord_token,
        body_len,
        json_body
        );

    if (len < 0 || (unsigned int)len >= sizeof(req_buf)) {
        fprintf(stderr, "[ERROR] Requested Buffer is too large");
        return -1;
    }

    printf("[INFO] Sending POST to %s\n", path);
    return client_send(client, req_buf, len);
}


int client_get(const Client *client, const char *path) {
    if (!client -> is_connected || !client -> ssl) {
        return -1;
    }

    char req_buf[4096];

    const int len = snprintf(req_buf, sizeof(req_buf),
        "GET %s HTTP/1.1\r\n"
        "Host: discord.com\r\n"
        "Authorization: Bot %s\r\n"
        "User-Agent: DiscordBot(BareBot, 1.0)\r\n"
        "Connection: close\r\n"
        "\r\n",
        path,
        config.discord_token);
    if (len < 0 || len >= sizeof(req_buf)) {
        fprintf(stderr, "[ERROR] GET request too large.\n");
        return -1;
    }

    printf("[INFO] Sending info to %s\n", path);
    return client_send(client, req_buf, len);
}



void client_cleanup(Client *client) {
    if (client -> ssl) {
        SSL_shutdown(client -> ssl);
        SSL_free(client -> ssl);
    }

    if (client -> context) {
        SSL_CTX_free(client -> context);
    }

    if (client -> socket_file_descriptor > 0) {
        close(client -> socket_file_descriptor);
    }

    client -> is_connected = false;
    printf("[INFO] Connection closed.\n");
}