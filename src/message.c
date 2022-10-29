#include <message-private.h>
#include <sdmessage.pb-c.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

struct message_t* message_create(){
    struct message_t *msg = malloc(sizeof(struct message_t));
    if(msg == NULL){
        return NULL;
    }

    message_t__init(&msg->content);
    return msg;
}

void message_destroy(struct message_t* msg){
    message_t__free_unpacked(&msg->content, NULL);
    free(msg);
}

int message_read_all(int socket, void* buffer, int size){
    int count,result = 0;
    while(count < size){
        result = read(socket, buffer + count, size - count);
        if(result <= 0){
            return -1;
        }
        else{
            count+=result;
        }
    }
    return count;
}

int message_write_all(int socket, void* buffer, int size){
    int result = 0;
    int len = size;
    while(size > 0){
        result = write(socket, buffer, size);
        if(result <= 0){
            return -1;
        }
        else{
            buffer+=result;
            size-=result;
        }
    }
    return len;
}