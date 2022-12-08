// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#ifndef _TREE_SKEL_PRIVATE_H
#define _TREE_SKEL_PRIVATE_H

#include <zookeeper/zookeeper.h>
#include <netinet/in.h>

struct rtree_t {
    struct sockaddr_in socket;
    int socket_num;
};

struct op_proc_t{
    int max_proc;
    int *in_progress;
};

void lock_queue();

void unlock_queue();

void lock_tree();

void unlock_tree();

void lock_op_proc();

void unlock_op_proc();

int tree_skel_put(struct request_t* request);

int tree_skel_del(struct request_t* request);

int place_in_queue(struct request_t *queue, struct request_t *request);

int tree_skel_zookeeper_init(char* address_port);

void tree_skel_my_watcher_func(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx);

void tree_skel_child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx);

#endif