#include <stdlib.h>
#include "list.h"

// cria uma lista vazia
list_t* create() {
    list_t *l = (list_t *)malloc(sizeof(list_t));
    l->begin = NULL;
    l->end = NULL;
    l->size = 0;

    return l;
}

// verifica se um elemento está na lista
int find(list_t *l, int x) {
    if (l == NULL) return -1;

    no_t *el = l->begin;
    while (el != NULL) {
        if (el->content == x) return 1;
        el = el->next;
    }

    return 0;
}

// aloca um elemento na lista
int push(list_t *l, int x) {
    if (l == NULL || find(l, x)) return 1;

    // criação do novo nó
    no_t *el = (no_t *)malloc(sizeof(no_t));
    el->content = x;
    el->next = NULL;

    // insere o nó na estrutura
    if (l->begin == NULL) l->begin = el;
    else l->end->next = el;

    // configura o novo fim da fila
    l->end = el;
    l->size ++;
    return 0;
}

// retira o elemento da lista
int pop(list_t *l, int x) {
    if (l == NULL) return 1;

    no_t *el = l->begin;
    no_t *before = NULL; 

    while (el != NULL) {
        // se achou o elemento para tirar
        if (el->content == x) {
            if (el == l->begin) { // se é o início da lista
                l->begin = l->begin->next;
                free(el);
            }
            else if (el == l->end) { // se é o fim da lista
                l->end = before;
                l->end->next = NULL;
                free(el);
            }
            else { // qualquer caso de remoção no meio
                before->next = el->next;
                free(el);
            }

            l->size --;
            return 0;
        }
        // se não encontrou, tenta o próximo
        else {
            before = el;
            el = el->next;
        }
    }

    return 0;
}

// apaga a lista da memória
void destroy(list_t *l) {
    if (l == NULL) return;

    // viaja por todos os nós apagando-os
    no_t *el = l->begin;
    while (el != NULL) {
        l->begin = el->next;
        free(el);
        el = l->begin;
    }

    free(l);
}