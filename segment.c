#include <stdio.h>
#include "segment.h"

// extrai do segmento lido as informações dos campos e dados
segment_t extractSegment(char *buffer) {
    segment_t segment;
    segment.syn = (short int)buffer[0];
    segment.ack = (short int)buffer[1];
    segment.clientId = (short int)buffer[2];

    for (int i = 0; i < LEN_DATA; i ++)
        segment.data[i] = buffer[i + LEN_HEADER];

    return segment;
}

// a partir dos dados a enviar, compõe a string do segmento para enviar
void composeSegment(char* segmentBuffer, short int syn, short int ack, short int clientId, char *data) {
    segmentBuffer[0] = (char)syn;
    segmentBuffer[1] = (char)ack;
    segmentBuffer[2] = (char)clientId;

    for (int i = 0; i < LEN_DATA; i ++)
        segmentBuffer[i + LEN_HEADER] = data[i];
}

// converte a estrutura de mensagem para a string do segmento para enviar
void convertSegment(char* segmentBuffer, segment_t message) {
    composeSegment(segmentBuffer, message.syn, message.ack, message.clientId, message.data);
}