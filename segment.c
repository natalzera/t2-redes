#include <stdio.h>
#include <string.h>
#include "segment.h"

// extrai do segmento lido as informações dos campos e dados
segment_t extractSegment(char *buffer) {
    segment_t segment;
    segment.clientId = (short int)buffer[0];
    strncpy(segment.data, buffer + LEN_HEADER, LEN_DATA);

    return segment;
}

// a partir dos dados a enviar, compõe a string do segmento para enviar
void composeSegment(char* segmentBuffer, short int clientId, char *data) {
    segmentBuffer[0] = (char)clientId;
    strncpy(segmentBuffer + LEN_HEADER, data, LEN_DATA);
}

// converte a estrutura de mensagem para a string do segmento para enviar
void convertSegment(char* segmentBuffer, segment_t message) {
    composeSegment(segmentBuffer, message.clientId, message.data);
}