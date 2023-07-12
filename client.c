// bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// bibliotecas para os sockets
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

// bibliotecas para a estrutura dos segmentos
#include "segment.h"
#define MAX_CHARS 64000

// endereço IP e porta do servidor para conexão
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12600

// lê envia a mensagem desejada para o servidor
void* sendMessage(void *args) {
    char totalMsg[MAX_CHARS];
    char buffer[LEN_HEADER + LEN_DATA];
    segment_t message;

    // extrai os argumentos
    int thisSocket = ((int *)args)[0];
    message.clientId = ((int *)args)[1];

    while (1) {
        // lê a mensagem inteira
        memset(totalMsg, 0, sizeof(totalMsg));
        fgets(totalMsg, sizeof(totalMsg), stdin);
        totalMsg[strcspn(totalMsg, "\n")] = '\0';

        // verifica se precisa dividir em mensagens menores
        int numMessages = (int)(strlen(totalMsg) / (LEN_DATA - 1)) + 1;
        for (int i = 0; i < numMessages; i ++) {
            memset(message.data, 0, LEN_DATA);
            strncpy(message.data, totalMsg + (i * (LEN_DATA - 1)), LEN_DATA - 1);
            // marca o final da mensagem
            if (i < numMessages - 1) message.data[LEN_DATA-1] = '\0';
            else message.data[strlen(totalMsg) % (LEN_DATA - 1)] = '\0';

            memset(buffer, 0, LEN_HEADER + LEN_DATA);
            convertSegment(buffer, message);
            write(thisSocket, buffer, LEN_HEADER + LEN_DATA);
        }

        // executa o comando de saída
        if (!strcmp(message.data, "quit")) {
            ((int *)args)[2] = 1; // informa que o socket foi fechado
            break;
        }
    }
}

int main() {
    // cria e configura o socket
    int thisSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (thisSocket < 0) {
        printf("Erro ao abrir o socket.\n");
        return 1;
    }

    // configura o endereço do servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // leitura de comandos antes da conexão
    segment_t message;
    while (1) {
        fgets(message.data, sizeof(message.data), stdin);
        if (!strcmp(message.data, "connect\n"))
            break;

        printf("Conete-se ao servidor antes.\n");
    }

    // conecta com o servidor (apenas os sockets)
    printf("Aguarde a conexão se estabelecer...\n");
    if (connect(thisSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        printf("Erro ao estabelecer conexão com o servidor.\n");
        return 1;
    }
    
    // solicita a conexão lógica e um id de usuário para o servidor
    char buffer[LEN_HEADER + LEN_DATA];
    message.data[strcspn(message.data, "\n")] = '\0';
    composeSegment(buffer, 0, 0, 0, message.data);
    write(thisSocket, buffer, sizeof(buffer));
    memset(buffer, 0, sizeof(buffer));

    // lê a resposta do servidor com o id de usuário
    read(thisSocket, buffer, sizeof(buffer));
    message = extractSegment(buffer);
    short int userId = message.clientId;
    printf("Conexão estabelecida. Usuário %d\n", userId);

    // cria uma thread para enviar mensagens ao servidor
    pthread_t threads;
    int argsMsg[3] = {
        thisSocket,
        userId,
        0 // se foi fechado o socket
    };
    pthread_create(&threads, NULL, sendMessage, argsMsg);

    // enquanto o socket não foi fechado, lê mensagens que o servidor mandou e extrai seus dados
    while (!argsMsg[2]) {
        memset(buffer, 0, LEN_HEADER + LEN_DATA);
        read(thisSocket, buffer, LEN_HEADER + LEN_DATA);
        message = extractSegment(buffer);
        printf("Resposta do servidor: %s\n", message.data);
    }

    // fecha o socket
    close(thisSocket);
    return 0;
}