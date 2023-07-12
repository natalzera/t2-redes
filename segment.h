#define LEN_HEADER 3 // SYN | ACK | clientId
#define LEN_DATA 15

// estrutura dos segmentos
typedef struct segment {
    short int syn, ack, clientId;
    char data[LEN_DATA];
} segment_t;

segment_t extractSegment(char *buffer);
void composeSegment(char* segmentBuffer, short int syn, short int ack, short int clientId, char *data);
void convertSegment(char* segmentBuffer, segment_t message);