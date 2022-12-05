// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <sdmessage.pb-c.h>
#include <tree.h>
#include <tree_skel.h>
#include <message-private.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <tree_skel-private.h>
#include <zookeeper/zookeeper.h>
#include <client_stub.h>  

struct tree_t *global_tree;
struct op_proc_t *op_proc;
struct request_t *queue_head;
int last_assigned;
int n_threads;

pthread_t* thread_ids;
pthread_mutex_t t_lock, q_lock, p_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t q_nonempty =  PTHREAD_COND_INITIALIZER;

typedef struct String_vector zoo_string; 
zoo_string* children_list;
static zhandle_t *zh;
static int is_connected;
static char *watcher_ctx = "ZooKeeper Data Watcher";

struct rtree_t* next_server;
char* server_address_port;


/* Inicia o skeleton da árvore.
* O main() do servidor deve chamar esta função antes de poder usar a
* função invoke().
* A função deve lançar N threads secundárias responsáveis por atender
* pedidos de escrita na árvore.
* Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
*/
int tree_skel_init(int N, char* address_port){
    global_tree = tree_create();

    if(global_tree == NULL){
        return -1;
    }

    last_assigned = 1;
    n_threads = N;
    op_proc = malloc(sizeof(struct op_proc_t));
    op_proc->max_proc = 0;
    op_proc->in_progress = calloc(50, sizeof(int)); //valor arbitrário, verificar se funciona
    
    queue_head = malloc(sizeof(struct request_t));
    queue_head = NULL;

    thread_ids = calloc(N, sizeof(pthread_t));

    for(int i = 0; i < N; i++){
        if(pthread_create(&thread_ids[i], NULL, &process_request, (void*) &queue_head) < 0){
            printf("Erro a criar thread!\n");
            return -1;
        }
    }

    server_address_port = address_port;
    children_list =	(zoo_string *) malloc(sizeof(zoo_string));
    //zh = malloc(sizeof(zhandle_t));

    return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){
    tree_destroy(global_tree);
    for(int i = 0; i < n_threads; i++){
        pthread_exit(&thread_ids[i]);
        // pthread_detach(thread_ids[i]);
    }
    free(thread_ids);
    free(op_proc->in_progress);
    free(op_proc);
    free(queue_head);

    zookeeper_close(zh);
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
    struct request_t *request;
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
            printf("Operacao de escrita mais recente, com numero de ordem %d\n", last_assigned);
            last_assigned++;

            request = malloc(sizeof(struct request_t));
            request->op = 0;
            request->op_n = last_assigned;
            request->key = msg->content.key;
            request->data = NULL;
            request->next = NULL;

            msg->content.opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.op_n = last_assigned;

            lock_queue();

            if(queue_head == NULL){
                queue_head = request;
            }
            else{
                struct request_t *queue = queue_head;
                place_in_queue(queue, request);  
            }

            pthread_cond_signal(&q_nonempty);
            unlock_queue();
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
                msg->content.data.data = data->data;
                msg->content.data.len = data->datasize;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_PUT:
            printf("Operacao de escrita mais recente, com numero de ordem %d\n", last_assigned);
            last_assigned++;

            request = malloc(sizeof(struct request_t));
            data = data_create2(msg->content.data.len, msg->content.data.data);
            request->op = 1;
            request->op_n = last_assigned;
            request->key = msg->content.key;
            request->data = data;
            request->next = NULL;

            msg->content.opcode = MESSAGE_T__OPCODE__OP_PUT + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.op_n = last_assigned;

            lock_queue();

            if(queue_head == NULL){
                queue_head = request;
            }
            else{
                struct request_t *queue = queue_head;
                place_in_queue(queue, request);  
            }

            pthread_cond_signal(&q_nonempty);
            unlock_queue();
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_GETKEYS: ;
            char **keys = tree_get_keys(global_tree);

            int i = 0;
            if(keys == NULL){
                printf("A arvore esta vazia!\n");
                msg->content.opcode = MESSAGE_T__OPCODE__OP_ERROR;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
                return 0;
            }
            else{
                msg->content.n_keys = tree_size(global_tree);
                msg->content.keys = keys;
                msg->content.opcode = MESSAGE_T__OPCODE__OP_GETKEYS + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_KEYS;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_GETVALUES: ;
            char **values = (char **)tree_get_values(global_tree);
            if(values == NULL){
                printf("A arvore esta vazia!\n");
                msg->content.opcode = MESSAGE_T__OPCODE__OP_ERROR;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_NONE;
                return 0;
            }
            else{
                msg->content.n_values = tree_size(global_tree);
                msg->content.values = values; 
                msg->content.opcode = MESSAGE_T__OPCODE__OP_GETVALUES + 1;
                msg->content.c_type = MESSAGE_T__C_TYPE__CT_VALUES;
            }
            return 0;
        break;

        case MESSAGE_T__OPCODE__OP_VERIFY:
            msg->content.opcode = MESSAGE_T__OPCODE__OP_VERIFY + 1;
            msg->content.c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            msg->content.result = verify(msg->content.op_n);
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

/* Função da thread secundária que vai processar pedidos de escrita.
*/
void * process_request (void *params){
    int i = 0;
    while(1){
        lock_queue();

        while(queue_head == NULL){
            pthread_cond_wait(&q_nonempty, &q_lock);
        }
        struct request_t *request = queue_head;

        lock_op_proc();
        while(op_proc->in_progress[i] != 0){
            i++;
        }
        op_proc->in_progress[i] = request->op_n;
        i = 0;
        unlock_op_proc();

        if(request->op == 0){
            if(tree_skel_del(request) != 0){
                printf("Ocorreu um erro na eliminacao de uma entrada!\n"); 
            }
        }
        else if(request->op == 1){
            if(tree_skel_put(request) != 0){
                printf("Ocorreu um erro na insercao de uma entrada!\n"); 
            }
        }
        else{
            printf("Valor de operacao invalido!\n");
        }

        if(request->next == NULL){
            queue_head = NULL;
        }
        else{
            queue_head = queue_head->next;
        }

        lock_op_proc();
        while(op_proc->in_progress[i] != request->op_n){
            i++;
        }
        op_proc->in_progress[i] = 0;
        if(request->op_n > op_proc->max_proc){
            op_proc->max_proc = request->op_n;
        }
        unlock_op_proc();

        unlock_queue();
    }
}

/* Verifica se a operação identificada por op_n foi executada.
*/
int verify(int op_n){
    lock_op_proc();
    int result = op_proc->max_proc;
    unlock_op_proc(); 
    return op_n <= result;
}

void lock_queue(){
    if(pthread_mutex_lock(&q_lock) < 0){
        perror("Ocorreu um erro a bloquear o mutex da fila!");
    }
}

void unlock_queue(){
    if(pthread_mutex_unlock(&q_lock) < 0){
        perror("Ocorreu um erro a desbloquear o mutex da fila!");
    }
}

void lock_tree(){
    if(pthread_mutex_lock(&t_lock) < 0){
        perror("Ocorreu um erro a bloquear o mutex da arvore!");
    }
}

void unlock_tree(){
    if(pthread_mutex_unlock(&t_lock) < 0){
        perror("Ocorreu um erro a desbloquear o mutex da arvore!");
    }
}

void lock_op_proc(){
    if(pthread_mutex_lock(&p_lock) < 0){
        perror("Ocorreu um erro a bloquear o mutex da op_proc!");
    }
}

void unlock_op_proc(){
    if(pthread_mutex_unlock(&p_lock) < 0){
        perror("Ocorreu um erro a desbloquear o mutex da op_proc!");
    }
}

int tree_skel_put(struct request_t* request){
    lock_tree();
    int result = tree_put(global_tree, request->key, request->data);
    
    if(result == -1){
        printf("Ocorreu um erro a colocar a entrada %s!\n", request->key);
    }
    else{
        printf("Entrada %s inserida!\n", request->key);
    }
    unlock_tree();
    return result;
}

int tree_skel_del(struct request_t* request){
    lock_tree();
    int result = tree_del(global_tree, request->key);

    if(result == -1){
        printf("Nao foi encontrada a entrada %s!\n", request->key);
    }
    else{
        printf("Entrada %s eliminada!\n", request->key);
    }
    unlock_tree();
    return result;
}

int place_in_queue(struct request_t *queue, struct request_t *request){
    if(queue == NULL || request == NULL){
        printf("Ocorreu um erro a colocar o pedido na fila!\n");
        return -1;
    }
    if(queue->next == NULL){
        queue->next = request;
        return 0;
    }
    else{
        return place_in_queue(queue->next, request);
    }
}

int tree_skel_zookeeper_init(char* address_port){
        //zoo_string* children_list =	(zoo_string *) malloc(sizeof(zoo_string));
    zh = zookeeper_init(address_port, tree_skel_my_watcher_func, 2000, 0, 0, 0);
    if(zh == NULL){
		printf("Ocorreu um erro a conectar ao servidor Zookeeper!\n");
	    return -1;
	}
    else{
        is_connected = 1;
    }

    //sleep(3);
    if(is_connected){
        if(ZNONODE == zoo_exists(zh, "/chain", 0, NULL)){
            printf("O node /chain ainda nao existe\n");

            if(ZOK == zoo_create(zh, "/chain", NULL, -1, &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0)){
                printf("Foi criado o node /chain\n");
            }
            else{
                printf("Ocorreu um erro a criar o node /chain\n");
                return -1;
            }
        }
        else{
            char node_path[120] = "";
            strcat(node_path,"/chain"); 
            strcat(node_path,"/node"); 
            int new_path_len = 1024;
            char* new_path = malloc (new_path_len);
            //data a NULL? ip:porto

            char* new_address_port = "";
            
            if(ZOK != zoo_create(zh, node_path, address_port, 50, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE, new_path, new_path_len)){
                printf("Ocorreu um erro a criar o node %s\n", node_path);
                return -1;
            }
            else{
                printf("Node criado com o path %s \n", new_path);
            }

            char next_id[120] = "/chain/node9999";  
            char child[120] = "";
            if(ZOK != zoo_wget_children(zh, "/chain", &tree_skel_child_watcher, watcher_ctx, children_list)) {
				printf("Ocorreu um erro a definir uma vigia aos filhos de /chain\n");
			}
            else{
                for(int i = 0; i < children_list->count; i++){
                    strcpy(child, children_list->data[i]);
                    if(strcmp(child, new_path) > 0){
                        if(strcmp(child, next_id) < 0){
                            strcpy(next_id, child);
                        }
                    }
                }
                if(strcmp(next_id, "/chain/node9999") == 0){
                    next_server = NULL;
                }
                else{
                    int buf_size = 1024;
                    char* buffer = malloc(buf_size);
                    if(zoo_get(zh, next_id, 0, buffer, &buf_size, 0) != ZOK){
                        printf("Ocorreu um erro a obter o node %s!\n", next_id);
                        return -1;
                    }
                    
                    next_server = rtree_connect(buffer);
                }
            }
        }
    }
    return 0;
}

void tree_skel_my_watcher_func(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx){
	if (type == ZOO_SESSION_EVENT){
		if (state == ZOO_CONNECTED_STATE){
			is_connected = 1; 
		}
        else{
			is_connected = 0; 
		}
	}
}

void tree_skel_child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx){
    
}
