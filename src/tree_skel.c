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
    int result;
    switch (msg->content.opcode){
        case MESSAGE_T__OPCODE__OP_SIZE:
            msg->content.opcode = MESSAGE_T__OPCODE__OP_SIZE + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.size = tree_size(global_tree);
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_HEIGHT:
            msg->content.opcode = MESSAGE_T__OPCODE__OP_HEIGHT + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.height = tree_height(global_tree);
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_DEL:
        
        //de onde vem a entry?
          //  result = tree_del(global_tree, msg->content.data)
            //msg->content.opcode = MESSAGE_T__OPCODE__OP_HEIGHT + 1;
            //msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            //msg->content.height = tree_height(global_tree);
            return 0;
        break;
    
    default:
        break;
    }
}
