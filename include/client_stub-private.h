#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include <netinet/in.h>
#include "data.h"

struct rtree_t {
    struct sockaddr_in socket;
    int socket_num;
};
// extern struct rtree_t *tree;
#endif