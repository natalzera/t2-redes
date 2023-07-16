#include "list.h"
#define MAX_CHANNELS 15

// estrutura dos canais
typedef struct channel {
    char name[30];
    int userAdm;
    list_t *users, *usersMutted;
} channel_t;

typedef struct all_channels {
    channel_t channels[MAX_CHANNELS];
    short int numChannels;
} channels_t;

// funções disponíveis
channels_t* initChannels();
int createChannel(channels_t *c, char *channelName, int socketAdm);
int getChannel(channels_t *c, char *channelName, int socket);
int checkAdm(int clientSocket, char *channelNameAux);
int findC(channels_t *c, char *channelName, int socket);
int join(channels_t *c, char *channelName, int socket);
int quit(channels_t *c, char *channelName, int socket);
int transfer(channels_t *c, char *channelDest, char *channelOrigin, int socket);
int mute(channels_t *c, char *channelName, int socket);
int unmute(channels_t *c, char *channelName, int socket);
int isMutted(channels_t *c, char *channelName, int socket);
void destroyChannels(channels_t *c);