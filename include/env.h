#ifndef ENV_H
#define ENV_H



extern struct Config {
    char *discord_token;
    char *channelid;
} config;

void loadenv();



#endif