#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

typedef struct message_t{
    MessageT content;
} message_t;

struct message_t* message_create();

int message_read_all(int socket, void* buffer, int size);

int message_write_all(int socket, void* buffer, int size);
#endif