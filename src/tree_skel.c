#include "sdmessage.pb-c.h"
#include "tree.h"
#include <message-private.h>

struct rtree_t *global_tree;
/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init(){
    global_tree = tree_create();
    if(global_tree == NULL){
        return -1;
    }
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){
    tree_destroy(global_tree);
}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
*/
int invoke(struct message_t *msg){
    if(global_tree == NULL){
        return -1;
    }

    int result;
    struct data_t* data;
    switch (msg->content.opcode){
        case MESSAGE_T__OPCODE__OP_SIZE:
            msg->content.opcode = MESSAGE_T__OPCODE__OP_SIZE + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.result = tree_size(global_tree);
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_HEIGHT:
            msg->content.opcode = MESSAGE_T__OPCODE__OP_HEIGHT + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.result = tree_height(global_tree);
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_DEL:
            result = tree_del(global_tree, msg->content.key);

            if(result == -1){
                printf("Nao foi encontrada a entrada %s!\n", msg->content.key);
                msg->content.opcode = MESSAGE_T__OPCODE__OP_ERROR;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
                return 0;
            }
            else{
                printf("Entrada %s eliminada!\n", msg->content.key);
                msg->content.opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_GET:
            data = tree_get(global_tree, msg->content.key);
            if(data == NULL){
                printf("A entrada %s nao existe!\n", msg->content.key);
                msg->content.opcode = MESSAGE_T__OPCODE__OP_GET + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
                msg->content.data.data = NULL;
                msg->content.data.len = 0;
                return 0;
            }
            else{
                msg->content.opcode = MESSAGE_T__OPCODE__OP_GET + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_VALUE;
                msg->content.data.data = data;
                msg->content.data.len = data->datasize;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_PUT:
            data = data_create2(msg->content.data.len, msg->content.data.data);
            result = tree_put(global_tree, msg->content.key, data);

            if(result == -1){
                printf("Ocorreu um erro a colocar a entrada %s!\n", msg->content.key);
                msg->content.opcode = MESSAGE_T__OPCODE__OP_ERROR;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            else{
                msg->content.opcode = MESSAGE_T__OPCODE__OP_PUT + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_GETKEYS:
            char** keys = tree_get_keys(global_tree);
            if(keys == NULL){
                printf("A arvore esta vazia!\n");
                msg->content.opcode = MESSAGE_T__OPCODE__OP_ERROR;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
                return 0;
            }
            else{
                msg->content.keys = keys;
                msg->content.opcode = MESSAGE_T__OPCODE__OP_GETKEYS + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_KEYS;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_GETVALUES:
            void **values = tree_get_values(global_tree);
            if(values == NULL){
                printf("A arvore esta vazia!\n");
                msg->content.opcode = MESSAGE_T__OPCODE__OP_ERROR;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
                return 0;
            }
            else{
                msg->content.values = values; 
                msg->content.opcode = MESSAGE_T__OPCODE__OP_GETVALUES + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_VALUES;
            }
            return 0;
        break;


        case MESSAGE_T__OPCODE__OP_BAD:
            return -1;
        break;

        case MESSAGE_T__OPCODE__OP_ERROR:
            return -1;
        break; 

        default:
            return -1;
        break;
    }
    return -1;
}
