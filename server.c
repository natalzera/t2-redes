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

// informações sobre a conexão
#define PORT 12600
#define MAX_CONNECTIONS 2

// trata a transição de dados que ocorre em uma comunicação cliente-servidor
void* connection(void *args) {
    // pega os dados do cliente passados por parâmetro
    char buffer[LEN_HEADER + LEN_DATA];
    int clientSocket = ((int *)args)[0];
    short int idClient = ((int *)args)[1];

    while (1) {
        // limpa o buffer e faz a leitura da mensagem
        memset(buffer, 0, sizeof(buffer));
        read(clientSocket, buffer, sizeof(buffer));
        segment_t message = extractSegment(buffer);

        // se solicita fechar conexão
        if (!strcmp(message.data, "quit")) { // fechar conexão
            printf("Conexão encerrada com cliente %d\n", message.clientId);
            strcpy(message.data, "disconnected");
            composeSegment(buffer, 0, 0, -1, message.data);
            write(clientSocket, buffer, sizeof(buffer));
            break;
        }
        // se solicita alguma resposta do servidor
        else {
            if (!strcmp(message.data, "connect")) { // iniciar conexão (mandando o id do cliente)
                composeSegment(buffer, 0, 0, idClient, message.data);
            }
            else if (!strcmp(message.data, "ping")) { // responder o ping
                strcpy(message.data, "pong");
                composeSegment(buffer, 0, 0, -1, message.data);
            }
            else { // responde a mensagem lida
                printf("%d: %s\n", message.clientId, message.data);
                composeSegment(buffer, 0, 0, -1, message.data);
            }
            
            write(clientSocket, buffer, sizeof(buffer));
        }
    }

    // encerra a conexão
    close(clientSocket);
}

int main() {
    // cria e configura o socket
    int thisSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (thisSocket < 0) {
        printf("Erro ao abrir o socket.\n");
        return 1;
    }

    // configura o endereço do servidor
    struct sockaddr_in thisAddr;
    thisAddr.sin_family = AF_INET;
    thisAddr.sin_port = htons(PORT);
    thisAddr.sin_addr.s_addr = INADDR_ANY;

    // associa o socket com o endereço do servidor
    if (bind(thisSocket, (struct sockaddr *) &thisAddr, sizeof(thisAddr)) < 0) {
        printf("Erro ao fazer o bind.\n");
        return 1;
    }

    // aguarda por conexões de clientes
    listen(thisSocket, MAX_CONNECTIONS);
    printf("Aguardando conexões...\n");

    // gerencia novas conexões com clientes utilizando threads
    pthread_t threads[MAX_CONNECTIONS];
    int threadCount = 0, clientId = 0;
    struct sockaddr_in clientAddr;

    while (1) {
        // estabelece uma nova conexão
        int clientLen = sizeof(clientAddr);
        int clientSocket = accept(thisSocket, (struct sockaddr *) &clientAddr, &clientLen);
        if (clientSocket < 0) {
            printf("Erro ao aceitar a conexão.\n");
            return 1;
        }
        printf("Conexão estabelecida com cliente %d.\n", clientId);
    
        // cria uma nova thread para gerenciar conexão
        int argsConnection[2] = {
            clientSocket,
            clientId
        };
        if (pthread_create(&threads[threadCount], NULL, connection, argsConnection)) {
            printf("Erro ao criar a thread.\n");
            return 1;
        }
        clientId ++;

        // se o limite de conexões for atingido, espera pela finalização de uma
        if (++ threadCount >= MAX_CONNECTIONS) {
            pthread_join(threads[threadCount - MAX_CONNECTIONS], NULL);
            threadCount --;
        }
    }

    // encerra o socket do servidor
    close(thisSocket);
    return 0;
}