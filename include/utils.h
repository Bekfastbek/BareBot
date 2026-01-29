#ifndef UTILS_H
#define UTILS_H



void keygen(char *output);

extern struct Config {
    char *discord_token;
    char *channelid;
} config;

void loadenv();



#endif