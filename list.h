// estrutura dos dados
typedef struct no no_t;
struct no {
    int content;
    no_t *next;
};

typedef struct list {
    no_t *begin, *end;
    int size;
} list_t;

// funções do TAD
list_t* create();
int find(list_t *l, int x);
int push(list_t *l, int x);
int pop(list_t *l, int x);
void destroy(list_t *l);