#include <message-private.h>

struct message_t* message_create();

int message_read_all(int socket, void* buffer, int size);

int message_write_all(int socket, void* buffer, int size);