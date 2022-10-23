#include <client_stub.h>
#include <client_stub-private.h>
#include <network_client.h>
#include <message-private.h>
#include <sdmessage.pb-c.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct rtree_t *global_tree;
/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port){
    global_tree = malloc(sizeof(struct rtree_t));
    char *hostname = strtok(address_port, ':');
    int port = atoi(strtok(NULL, ':'));

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

//     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
// perror("Erro ao criar socket TCP");
// return -1;
// }
// // Preenche estrutura server para estabelecer conexão
// server.sin_family = AF_INET;
// server.sin_port = htons(atoi(argv[2]));
// if (inet_pton(AF_INET, argv[1], &server.sin_addr) < 1) {
// printf("Erro ao converter IP\n");
// close(sockfd);
// return -1;
// }
// // Estabelece conexão com o servidor definido em server
// if (connect(sockfd,(struct sockaddr *)&server, sizeof(server)) < 0) {
// perror("Erro ao conectar-se ao servidor");
// close(sockfd);
// return -1;
// }
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree){
    if(network_close(rtree) == -1){
        return -1;
    }
    free(global_tree);
    return 0;
}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_PUT;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
    msg->content.data.data = entry->value->data;
    msg->content.data.len = entry->value->datasize;
    network_send_receive(rtree, msg);
    message_t__free_unpacked(&msg->content, NULL);
    return 0;
    

}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key);

/* Função para remover um elemento da árvore. Vai libertar 
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key);

/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree){
    struct message_t *msg = message_create();
    if(msg == NULL){
        return -1;
    }

    msg->content.opcode = MESSAGE_T__OPCODE__OP_SIZE;
    msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
    msg = network_send_receive(rtree, msg);

    //return msg->content
}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree);

/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree);

/* Devolve um array de void* com a cópia de todas os values da árvore,
 * colocando um último elemento a NULL.
 */
void **rtree_get_values(struct rtree_t *rtree);
