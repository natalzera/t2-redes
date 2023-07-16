#include <stdlib.h>
#include <string.h>
#include "channel.h"

// inicializa a estrutura de canais
channels_t* initChannels() {
    channels_t *c = (channels_t *)malloc(sizeof(channels_t));
    c->numChannels = 0;
    return c;
}

// aloca um novo canal na estrutura
int createChannel(channels_t *c, char *channelName, int socketAdm) {
    // verifica se possui espaço para novos canais
    int channelIndex = c->numChannels;
    if (channelIndex >= MAX_CHANNELS)
        return 1;

    strcpy(c->channels[channelIndex].name, channelName);
    c->channels[channelIndex].users = create();
    c->channels[channelIndex].usersMutted = create();
    c->channels[channelIndex].userAdm = socketAdm;
    push(c->channels[channelIndex].users, socketAdm);
    
    c->numChannels ++;
    return 0;
}

// descobre o canal que o socket está inserido
int getChannel(channels_t *c, char *channelName, int socket) {

    for (int i = 0; i < c->numChannels; i ++) {
        if (find(c->channels[i].users, socket) == 1) {
            strcpy(channelName, c->channels[i].name);
            return 0;
        }
    }

    return 1; // se não está em nenhum canal
}

/**
 * @brief Checks if the socket is the admin of the channel
 * 
 * @param clientSocket the socket in question
 * @param channelNameAux the name of the channel in question
 * @return the number of the channel found or -1 if it`s not the admin
 */
int checkAdm(int clientSocket, char *channelNameAux) {

    int admFound = 0, channelNum = 0;
    for (int i = 0; i < MAX_CHANNELS; i++)
        if (strcmp(channels->channels[i].name, channelNameAux) == 0 && clientSocket == channels->channels[i].userAdm)
            return i;

    return -1;
}

// verifica se o socket está no canal
int findC(channels_t *c, char *channelName, int socket) {
    
    for (int i = 0; i < c->numChannels; i ++)
        if (!strcmp(channelName, c->channels[i].name))
            return find(c->channels[i].users, socket);

    return -1; // se o canal não existe
}

// insere um novo socket em um canal
int join(channels_t *c, char *channelName, int socket) {

    for (int i = 0; i < c->numChannels; i ++) {
        if (!strcmp(channelName, c->channels[i].name)) {
            // se o usuário já está no canal
            if (find(c->channels[i].users, socket) == 1)
                return 1;

            return push(c->channels[i].users, socket);
        }
    }

    // se o canal não existe, cria-o
    return createChannel(c, channelName, socket);
}

// remove o socket do canal
int quit(channels_t *c, char *channelName, int socket) {

    for (int i = 0; i < c->numChannels; i ++) {
        if (!strcmp(channelName, c->channels[i].name)) {
            // se o usuário não está no canal ou é adm
            if (find(c->channels[i].users, socket) != 1 || c->channels[i].userAdm == socket)
                return 1;

            return pop(c->channels[i].users, socket);
        }
    }

    return -1; // se o canal não existe
}

// transfere o socket de um canal para outro
int transfer(channels_t *c, char *channelDest, char *channelOrigin, int socket) {
    if (!quit(c, channelOrigin, socket))
        return join(c, channelDest, socket);
    return 1;
}

// muta um usuário do canal
int mute(channels_t *c, char *channelName, int socket) {
    
    for (int i = 0; i < c->numChannels; i ++) {
        if (!strcmp(channelName, c->channels[i].name)) {
            // se o usuário não está no canal ou já está mutado
            if (find(c->channels[i].users, socket) != 1 || find(c->channels[i].usersMutted, socket) == 1)
                return 1;

            return push(c->channels[i].usersMutted, socket);
        }
    }

    return -1; // se o canal não existe
}

// desmuta o usuário do canal
int unmute(channels_t *c, char *channelName, int socket) {

    for (int i = 0; i < c->numChannels; i ++) {
        if (!strcmp(channelName, c->channels[i].name)) {
            // se o usuário não está no canal ou nem está mutado
            if (find(c->channels[i].users, socket) != 1 || find(c->channels[i].usersMutted, socket) != 1)
                return 1;

            return pop(c->channels[i].usersMutted, socket);
        }
    }

    return -1; // se o canal não existe
}

// verifica se o usuário está mutado no canal
int isMutted(channels_t *c, char *channelName, int socket) {
    
    for (int i = 0; i < c->numChannels; i ++)
        if (!strcmp(channelName, c->channels[i].name))
            return find(c->channels[i].usersMutted, socket);

    return -1; // se o canal não existe
}

// desaloca a estrutura de canais
void destroyChannels(channels_t *c) {
    if (c == NULL) return;

    for (int i = 0; i < c->numChannels; i ++) {
        destroy(c->channels[i].users);
        destroy(c->channels[i].usersMutted);
        c->channels[i].users = NULL;
        c->channels[i].usersMutted = NULL;
    }

    free(c);
    c = NULL;
}