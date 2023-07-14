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
#include "channel.h"

// bibliotecas para a estrutura dos segmentos
#include "segment.h"
#include "nick.h"
extern char nicknames[MAX_USERS_NICK][30];

// informações sobre a conexão
#define PORT 12600
#define MAX_REPLY 5
#define MAX_CONNECTIONS 5
list_t *allSockets;
channels_t *channels;

// envia a mensagem desejada para todos os clientes conectados
void sendAll(char *buffer, char *channelName) {
    int bytesWritten, numReply;
    no_t *aux = allSockets->begin;

    while (aux != NULL) {
        // verifica se o socket está no canal de envio da mensagem
        if (channelName != NULL && findC(channels, channelName, aux->content) != 1) {
            aux = aux->next;
            continue;
        }

        // tenta enviar para cada usuário 5 vezes a mensagem caso ela não seja enviada
        numReply = 0;
        do {
            bytesWritten = (int)write(aux->content, buffer, LEN_HEADER + LEN_DATA);
            numReply ++;
        } while (numReply < 5 && bytesWritten <= 0);
        
        // se não conseguiu mandar a mensagem nas tentativas, encerra a conexão
        if (bytesWritten <= 0) {
            printf("A mensagem não pôde ser enviada para o socket %d\n", aux->content);
            pop(allSockets, aux->content);
            close(aux->content);
        }

        aux = aux->next;
    }
}

// trata a transição de dados que ocorre em uma comunicação cliente-servidor
void* connection(void *args) {
    // pega os dados do cliente passados por parâmetro
    char buffer[LEN_HEADER + LEN_DATA];
    int clientSocket = ((int *)args)[0];
    short int idClient = ((int *)args)[1];

    char userName[30], channelName[30], channelNameAux[30];
    while (1) {
        // limpa o buffer e faz a leitura da mensagem
        memset(buffer, 0, LEN_HEADER + LEN_DATA);
        read(clientSocket, buffer, LEN_HEADER + LEN_DATA);

        // extrai os dados da mensagem recebida
        segment_t message = extractSegment(buffer);
        strcpy(userName, nicknames[message.clientId % MAX_USERS_NICK]);

        // se solicita fechar conexão
        if (!strcmp(message.data, "/quit")) { // fechar conexão
            printf("Conexão encerrada com %s\n", userName);
            strcpy(message.data, "disconnected");
            composeSegment(buffer, -1, message.data);
            write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
            break;
        }
        // se solicita iniciar conexão (mandando o id do cliente)
        else if (!strcmp(message.data, "/connect")) {
            composeSegment(buffer, idClient, message.data);
            write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
            
            // envia os dados da tabela de nomes de usuário atualizados
            strcpy(message.data, "/nickname ");
            for (int i = 0; i < idClient; i ++) {
                message.clientId = i;
                memset(message.data + 10, 0, LEN_DATA - 10);
                strcat(message.data, nicknames[i % MAX_USERS_NICK]);
                convertSegment(buffer, message);
                write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
            }
        }
        // se solicitou o fechamento do servidor (mas falhou)
        else if (!strcmp(message.data, "/close")) {
            strcpy(message.data, "not closed");
            composeSegment(buffer, -1, message.data);
            write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
            printf("Conexão encerrada com o último usuário (tentou fechar o servidor)\n");
            break;
        }
        // responder o ping
        else if (!strcmp(message.data, "/ping")) {
            printf("ping de %s\n", userName);
            strcpy(message.data, "pong");
            composeSegment(buffer, -1, message.data);
            write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
        }
        // mudar nome de usuário
        else if (strstr(message.data, "/nickname")) {
            strcpy(nicknames[message.clientId % MAX_USERS_NICK], message.data + 10);
            printf("Usuário %d mudou seu nick para %s\n", message.clientId, message.data + 10);
            sendAll(buffer, NULL);

            // avisa o usuário a mudança
            strcpy(message.data, "Nickname alterado para ");
            strcat(message.data, nicknames[message.clientId % MAX_USERS_NICK]);
            composeSegment(buffer, -1, message.data);
            write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
        }
        // entrar em um canal
        else if (strstr(message.data, "/join")) {
            strcpy(channelName, message.data + 6);
            if (getChannel(channels, channelNameAux, clientSocket) == 1) {
                join(channels, channelName, clientSocket);
                printf("%s entrou no canal %s\n", userName, channelName);
                
                // avisa os usuários do novo canal
                strcpy(message.data, userName);
                strcat(message.data, " entrou no canal ");
                strcat(message.data, channelName);
                composeSegment(buffer, -1, message.data);
                sendAll(buffer, channelName);
            }
            // se já estava em um canal, então transfere pra outro
            else if (!transfer(channels, channelName, channelNameAux, clientSocket)) {
                printf("%s saiu do canal %s e entrou no %s\n", userName, channelNameAux, channelName);
                
                // avisa os usuários do novo canal
                strcpy(message.data, userName);
                strcat(message.data, " entrou no canal ");
                strcat(message.data, channelName);
                composeSegment(buffer, -1, message.data);
                sendAll(buffer, channelName);

                // avisa os usuários do antigo canal
                strcpy(message.data, userName);
                strcat(message.data, " saiu do canal");
                composeSegment(buffer, -1, message.data);
                sendAll(buffer, channelNameAux);
            }
            // se não foi possível entrar ou transferir
            else {
                strcpy(message.data, "Não é possível sair do canal ");
                strcat(message.data, channelNameAux);
                composeSegment(buffer, -1, message.data);
                write(clientSocket, buffer, LEN_HEADER + LEN_DATA);
            }
        }
        // enviar mensagem a todos do canal
        else {
            // se não está em nenhum canal, envia a todos usuários conectados
            if (getChannel(channels, channelName, clientSocket) == 1) {
                printf("%s: %s\n", userName, message.data);
                sendAll(buffer, NULL);
            }
            // se está no canal, mas está mutado
            else if (isMutted(channels, channelName, clientSocket) == 1) {
                printf("[%s] (mutted)%s: %s\n", channelName, userName, message.data);
            }
            // se está no canal e desmutado, envia a todos do canal
            else {
                printf("[%s] %s: %s\n", channelName, userName, message.data);
                sendAll(buffer, channelName);
            }
        }   
    }

    // encerra a conexão
    pop(allSockets, clientSocket);
    close(clientSocket);
    pthread_exit(NULL);
}

int main() {
    setUsersNickDefault();

    // cria e configura o socket
    int thisSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (thisSocket < 0) {
        printf("Erro ao abrir o socket\n");
        return 1;
    }

    // configura o endereço do servidor
    struct sockaddr_in thisAddr;
    thisAddr.sin_family = AF_INET;
    thisAddr.sin_port = htons(PORT);
    thisAddr.sin_addr.s_addr = INADDR_ANY;

    // associa o socket com o endereço do servidor
    if (bind(thisSocket, (struct sockaddr *) &thisAddr, sizeof(thisAddr)) < 0) {
        printf("Erro ao fazer o bind\n");
        return 1;
    }

    // aguarda por conexões de clientes
    listen(thisSocket, MAX_CONNECTIONS);
    printf("Aguardando conexões...\n");

    // gerencia novas conexões e canais com clientes utilizando threads
    pthread_t threads[MAX_CONNECTIONS];
    int threadCount = 0, clientId = 0;
    struct sockaddr_in clientAddr;
    allSockets = create();
    channels = initChannels();
    
    do {
        // estabelece uma nova conexão
        int clientLen = sizeof(clientAddr);
        int clientSocket = accept(thisSocket, (struct sockaddr *) &clientAddr, &clientLen);
        if (clientSocket < 0) {
            printf("Erro ao aceitar a conexão\n");
            return 1;
        }
        // se o servidor estava vazio
        if (clientId > 0 && allSockets->size <= 0) {
            printf("Servidor desconectando...\n");
            close(clientSocket);
            break;
        }
        printf("Conexão estabelecida com usuário %d\n", clientId);
        push(allSockets, clientSocket);

        // cria uma nova thread para gerenciar conexão
        int argsConnection[2] = {
            clientSocket,
            clientId
        };
        if (pthread_create(&threads[threadCount], NULL, connection, argsConnection)) {
            printf("Erro ao criar a thread\n");
            return 1;
        }
        clientId ++;

        // se o limite de conexões for atingido, espera pela finalização de uma
        if (++ threadCount >= MAX_CONNECTIONS) {
            pthread_join(threads[threadCount - MAX_CONNECTIONS], NULL);
            threadCount --;
        }
    } while (allSockets->size > 0);

    // encerra o socket do servidor e suas threads utilizadas
    close(thisSocket);
    destroy(allSockets);
    destroyChannels(channels);
    for (int i = 0; i < threadCount; i ++)
        pthread_join(threads[i], NULL);

    return 0;
}