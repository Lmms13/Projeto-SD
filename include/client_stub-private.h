// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include <netinet/in.h>
#include "data.h"

struct rtree_t {
    struct sockaddr_in socket;
    int socket_num;
};

#endif