#define LEN_HEADER 1 // clientId
#define LEN_DATA 4096

// estrutura dos segmentos
typedef struct segment {
    short int clientId;
    char data[LEN_DATA];
} segment_t;

segment_t extractSegment(char *buffer);
void composeSegment(char* segmentBuffer, short int clientId, char *data);
void convertSegment(char* segmentBuffer, segment_t message);