#ifndef UTILS_H
#define UTILS_H



void keygen(char *output);

extern struct config_env {
    char *discord_token;
    char *channelid;
} config;

void loadenv();



#endif