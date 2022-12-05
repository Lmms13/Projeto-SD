// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include <netinet/in.h>
#include <zookeeper/zookeeper.h>
#include "data.h"

struct rtree_t {
    struct sockaddr_in socket;
    int socket_num;
};

int client_stub_zookeeper_init(char* address_port);

struct rtree_t *rtree_connect_head();

struct rtree_t *rtree_connect_tail();

void client_stub_my_watcher_func(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx);

void client_stub_child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx);

#endif