// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <client_stub.h>
#include <client_stub-private.h>
#include <network_client.h>
#include <message-private.h>
#include <sdmessage.pb-c.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <zookeeper/zookeeper.h>
#include <unistd.h>

struct rtree_t *global_tree;

// typedef struct String_vector zoo_string; 
// zoo_string* children_list;
// static zhandle_t *zh;
// static int is_connected;
// static char *watcher_ctx = "ZooKeeper Data Watcher";

/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port){
    global_tree = malloc(sizeof(struct rtree_t));
    if(global_tree == NULL){
        return NULL;
    }

    char *hostname = strtok((char*) address_port, ":");
    int port = atoi(strtok(NULL, ":"));

    global_tree->socket.sin_port = htons(port);
    global_tree->socket.sin_family = AF_INET;

    if(inet_pton(AF_INET, hostname, &global_tree->socket.sin_addr) < 1) {
        printf("Erro ao converter IP\n");
        return NULL;
    }

    if(network_connect(global_tree) == -1){
        free(global_tree);
        return NULL;
    }
    return global_tree;
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree){
    if(rtree == NULL){
        return -1;
    }

    if(network_close(rtree) == -1){
        return -1;
    }

    free(rtree);
    return 0;
}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
    if(rtree == NULL || entry == NULL){
        return -1;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_PUT;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_ENTRY;

    msg->content.data.data = entry->value->data;
    msg->content.data.len = entry->value->datasize;
    msg->content.key = entry->key;
    
    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return -1;
    }

    if(msg->content.opcode == MESSAGE_T__OPCODE__OP_ERROR){
        printf("Ocorreu um erro a colocar a entrada %s na arvore!\n", entry->key);
        message_destroy(msg);
        return -1;
    }

    return msg->content.op_n;
}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key){
    if(rtree == NULL || key == NULL){
        return NULL;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return NULL;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_GET;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_KEY;

    msg->content.key = key;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return NULL;
    }

    if(msg->content.c_type == MESSAGE_T__C_TYPE__CT_NONE){
        printf("A entrada %s nao existe na arvore!\n", key);
        message_destroy(msg);
        return NULL;
    }

    struct data_t* data = data_create2(msg->content.data.len, msg->content.data.data);

    if(data == NULL){
        printf("Ocorreu um erro!\n");
        return NULL;
    }

    free(msg);
    return data;
}

/* Função para remover um elemento da árvore. Vai libertar 
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key){
    if(rtree == NULL || key == NULL){
        return -1;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_DEL;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_KEY;

    msg->content.key = key;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return -1;
    }

    if(msg->content.opcode == MESSAGE_T__OPCODE__OP_ERROR){
        printf("A entrada %s nao existe na arvore!\n", key);
        message_destroy(msg);
        return -1;
    }

    return msg->content.op_n;
}

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree){
    if(rtree == NULL){
        return -1;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_SIZE;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return -1;
    }

    return msg->content.result;
}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree){
    if(rtree == NULL){
        return -1;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_HEIGHT;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return -1;
    }

    printf("A arvore tem %d niveis de altura\n", msg->content.result);

    return msg->content.result;
}

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree){
    if(rtree == NULL){
        return NULL;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return NULL;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return NULL;
    }

    printf("%ld chaves encontradas\n", msg->content.n_keys);

    return msg->content.keys;
}

/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree){
    if(rtree == NULL){
        return NULL;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return NULL;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_GETVALUES;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return NULL;
    }

    printf("%ld valores encontrados\n", msg->content.n_values);

    return (void**) msg->content.values;
}

/* Verifica se a operação identificada por op_n foi executada.
*/
int rtree_verify(struct rtree_t *rtree, int op_n){
      if(rtree == NULL){
        return -1;
    }

    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_VERIFY;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
    msg->content.op_n = op_n;

    msg = network_send_receive(rtree, msg);

    if(msg == NULL){
        printf("Ocorreu um erro!\n");
        return -1;
    }

    if(msg->content.c_type == MESSAGE_T__C_TYPE__CT_NONE){
        printf("Ocorreu um erro na verificação!\n");
        return -1;
    }

    return msg->content.result;
}
