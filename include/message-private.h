// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include <sdmessage.pb-c.h>

typedef struct message_t{
    MessageT content;
} message_t;

struct message_t* message_create();

void message_destroy(struct message_t* msg);

int message_read_all(int socket, void* buffer, int size);

int message_write_all(int socket, void* buffer, int size);

struct request_t {
int op_n; //o número da operação
int op; //a operação a executar. op=0 se for um delete, op=1 se for um put
char* key; //a chave a remover ou adicionar
struct data_t *data; // os dados a adicionar em caso de put, ou NULL em caso de delete
struct request_t *next;
// struct message_t *message;
};

#endif