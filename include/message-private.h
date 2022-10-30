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
#endif