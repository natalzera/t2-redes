#include "tables.h"
#include <stdio.h>
#include <string.h>

// configuração dos nomes de usuários padrão
char nicknames[MAX_USERS_NICK][30];
void setUsersNickDefault() {
    char nickAux[30];
    for (int i = 0; i < MAX_USERS_NICK; i ++) {
        sprintf(nickAux, "%d", i);
        strcpy(nicknames[i], nickAux);
    }
}

int idsSockets[MAX_USERS_NICK];
