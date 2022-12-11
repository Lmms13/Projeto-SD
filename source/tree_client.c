// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <client_stub.h>
#include <client_stub-private.h>
#include <data.h>
#include <entry.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

struct rtree_t *head;
struct rtree_t *tail;

typedef struct String_vector zoo_string; 
zoo_string* children_list;
static zhandle_t *zh;
static int is_connected;
static char *watcher_ctx = "ZooKeeper Data Watcher";


int main(int argc, char *argv[]){
    int value = -1;
    if(argc != 2){
        printf("Incluir o IP ou nome do servidor e o numero do porto TCP do servidor!\nExemplo de utilizacao: ./bin/tree-client<server>:<port>\n");
        return -1;
    }

    if(client_stub_zookeeper_init(argv[1]) == -1){
        printf("Ocorreu um erro a iniciar o Zookeeper!\n");
        return -1;
    }

    if(head == NULL){
        printf("Ocorreu um erro a associar o cliente ao servidor da cabeca!\n");
        return -1;
    }

    if(tail == NULL){
        printf("Ocorreu um erro a associar o cliente ao servidor da cauda!\n");
        return -1;
    }

    while(1){
        char input[100];
        printf("\nInsira um dos seguintes comandos:\n");
        printf("put <key> <data>\n");
        printf("get <key>\n");
        printf("del <key>\n");
        printf("size\n");
        printf("height\n");
        printf("getkeys\n");
        printf("getvalues\n");
        printf("verify <op_n>\n");
        printf("quit\n\n");

        fgets(input, 100, stdin);
        char *token = strtok(input, " \n");

        if(strcmp(token, "put") == 0){
            char *key = strtok(NULL, " ");
            char *input_data = strtok(NULL, "\n");
            if(key == NULL || input_data == NULL){
                printf("O comando put tem de incluir uma chave e dados!\n");
                printf("Exemplo: put <key> <data>\n");
                printf("-----------------------\n");
                continue;
            }

            int size = strlen(input_data);

            struct data_t *data = data_create2(size, input_data);
            struct entry_t *entry = entry_create(key, data);

            value = rtree_put(head, entry);
            if(value > 0){
                sleep(1);
                if(!rtree_verify(tail, value)){
                    rtree_put(head, entry);
                }
                else{
                    printf("Pedido de operacao colocado na fila com o numero de ordem %d\n", value);
                }
            }
            else{
                printf("Entrada nao inserida.");
            }
            printf("-----------------------\n");
        }

        else if(strcmp(token, "get") == 0){
            char *key = strtok(NULL, " \n");
            if(key == NULL){
                printf("O comando get tem de incluir uma chave!\n");
                printf("Exemplo: get <key>\n");
                printf("-----------------------\n");
                continue;
            }

            struct data_t *data = rtree_get(tail, key);
            if(data != NULL){
                char data_val[data->datasize + 1];
                memcpy(data_val, (char*) data->data, data->datasize);
                data_val[data->datasize] = '\0';
                printf("Chave %s tem valor %s\n", key, data_val);
            }
            data_destroy(data);
            printf("-----------------------\n");
        }

        else if(strcmp(token, "del") == 0){
            char *key = strtok(NULL, " \n");
            if(key == NULL){
                printf("O comando del tem de incluir uma chave!\n");
                printf("Exemplo: del <key>\n");
                printf("-----------------------\n");
                continue;
            }

            value = rtree_del(head, key);
            if(value > 0){
                sleep(1);
                if(!rtree_verify(tail, value)){
                    rtree_del(head, key);
                }
                else{
                    printf("Pedido de operacao colocado na fila com o numero de ordem %d\n", value);
                }
            }
            else{
                printf("Entrada nao inserida.");
            }
            printf("-----------------------\n");
        }

        else if(strcmp(token, "size") == 0){
            printf("A arvore tem %d entradas\n", rtree_size(tail));
            printf("-----------------------\n");
        }

        else if(strcmp(token, "height") == 0){
            rtree_height(tail);
            printf("-----------------------\n");
        }

        else if(strcmp(token, "getkeys") == 0){
            char **keys = rtree_get_keys(tail);
            if(keys == NULL){
                if(rtree_size(tail) == 0){
                    printf("A arvore esta vazia\n");
                }
                else{
                    printf("Ocorreu um erro a obter as chaves!\n");
                }
                printf("-----------------------\n");
                continue;
            }
            else{
                int tree_size =  rtree_size(tail);
                for(int i = 0; i < tree_size; i++){
                    if(keys[i] != NULL){
                        printf("%s\n", keys[i]);
                    }
                }

                for(int i = 0; i < tree_size; i++){
                    if(keys[i] != NULL){
                        free(keys[i]);
                    }
                }
                free(keys);
            }
            printf("-----------------------\n");
        }

        else if(strcmp(token, "getvalues") == 0){
            char **values = (char **) rtree_get_values(tail);
            if(values == NULL){
                if(rtree_size(tail) == 0){
                    printf("A arvore esta vazia\n");
                }
                else{
                    printf("Ocorreu um erro a obter as chaves!\n");
                }
                printf("-----------------------\n");
                continue;
            }
            else{
                int tree_size =  rtree_size(tail);
                for(int i = 0; i < tree_size; i++){
                    if(values[i] != NULL){
                        printf("%s\n", values[i]);
                    }
                }

                for(int i = 0; i < tree_size; i++){
                    if(values[i] != NULL){
                        free(values[i]);
                    }
                }
                free(values);
            }
            printf("-----------------------\n");
        }

        else if(strcmp(token, "verify") == 0){
            char* input = strtok(NULL, " \n");
            if(input == NULL){
                printf("O comando verify tem de incluir um numero!\n");
                printf("Exemplo: get <op_n>\n");
                printf("-----------------------\n");
                continue;
            }
            int op_n = atoi(input);
            int result = rtree_verify(tail, op_n);
            if(result == 0){
                printf("A operacao %d ainda nao foi executada\n", op_n);
            }
            else if(result == 1){
                printf("A operacao %d ja foi executada\n", op_n);
            }
            else{
                printf("Ocorreu um erro!\n");
            }
            printf("-----------------------\n");
        }

        else if(strcmp(token, "quit") == 0){
            printf("A terminar a execucao...\n");
            if(rtree_disconnect(head) == -1){
                printf("Ocorreu um erro a desconectar do servidor da cabeca!\n");
            }
            if(rtree_disconnect(tail) == -1){
                printf("Ocorreu um erro a desconectar do servidor da cauda!\n");
            }
            printf("-----------------------\n");
            break;
        }

        else{
            printf("Comando nao reconhecido!\n");
        }      
    }  
    return 0;  
}


int client_stub_zookeeper_init(char* address_port){
    children_list =	(zoo_string *) malloc(sizeof(zoo_string));

    zh = zookeeper_init(address_port, client_stub_my_watcher_func, 2000, 0,0,0);
    if(zh == NULL){
		printf("Ocorreu um erro a conectar ao servidor Zookeeper!\n");
	    return -1;
	}
    else{
        is_connected = 1;
    }

    if(is_connected){
        if(zoo_wget_children(zh, "/chain", client_stub_child_watcher, watcher_ctx, children_list) != ZOK){
            printf("Ocorreu um erro a definir uma vigia em /chain\n");
            return -1;
        }


        head = rtree_connect_head();
        tail = rtree_connect_tail();
    }
    return 0;
}

void client_stub_my_watcher_func(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx){
	if (type == ZOO_SESSION_EVENT){
		if (state == ZOO_CONNECTED_STATE){
			is_connected = 1; 
		}
        else{
			is_connected = 0; 
		}
	}
}

void client_stub_child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx){
    if(state == ZOO_CONNECTED_STATE){
		if(type == ZOO_CHILD_EVENT){
            if(zoo_wget_children(zh, "/chain", client_stub_child_watcher, watcher_ctx, children_list) != ZOK){
                printf("Ocorreu um erro a definir uma vigia em /chain\n");
                return;
            }

            head = rtree_connect_head();
            tail = rtree_connect_tail();

            if(zoo_wget_children(zh, "/chain", client_stub_child_watcher, watcher_ctx, children_list) != ZOK){
                printf("Ocorreu um erro a definir uma vigia em /chain\n");
                return;
            }
        }
    }
}

struct rtree_t *rtree_connect_head(){
    char min[120] = "node9999999999";
    char child[120] = "";

    int buf_size = 1024;
    char* buffer = malloc(buf_size);

    for(int i = 0; i < children_list->count; i++){
        strcpy(child, children_list->data[i]);
        if(strcmp(child, min) < 0){
            strcpy(min, child);
        } 
    }

    char head_path[120] = "/chain/";
    strcat(head_path, min);

    printf("-----------------------\n");
    printf("CABECA: %s\n", head_path);

    if(zoo_get(zh, head_path, 0, buffer, &buf_size, 0) != ZOK){
        printf("Ocorreu um erro a obter o servidor da cabeca!\n");
        return NULL;
    }

    head = rtree_connect(buffer);
    return head;
}

struct rtree_t *rtree_connect_tail(){
    char max[120] = "node0000000000";
    char child[120] = "";

    int buf_size = 1024;
    char* buffer = malloc(buf_size);

    for(int i = 0; i < children_list->count; i++){
        strcpy(child, children_list->data[i]);
        if(strcmp(child, max) > 0){
            strcpy(max, child);
        } 
    }

    char tail_path[120] = "/chain/";
    strcat(tail_path, max);

    printf("CAUDA: %s\n", tail_path);
    printf("-----------------------\n");

    if(zoo_get(zh, tail_path, 0, buffer, &buf_size, 0) != ZOK){
        printf("Ocorreu um erro a obter o servidor da cauda!\n");
        return NULL;
    }

    tail = rtree_connect(buffer);
    return tail;
}