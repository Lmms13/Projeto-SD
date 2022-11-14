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


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Incluir o IP ou nome do servidor e o numero do porto TCP do servidor!\nExemplo de utilizacao: ./bin/tree-client<server>:<port>\n");
        return -1;
    }

    struct rtree_t *tree;
    tree = rtree_connect(argv[1]);
    if(tree == NULL){
        printf("Ocorreu um erro a associar o cliente ao servidor!\n");
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
        printf("quit\n\n");

        fgets(input, 100, stdin);
        char *token = strtok(input, " \n");

        if(strcmp(token, "put") == 0){
            char *key = strtok(NULL, " ");
            char *input_data = strtok(NULL, "\n");
            if(key == NULL || input_data == NULL){
                printf("O comando put tem de incluir uma chave e dados!\n");
                printf("Exemplo: put <key> <data>\n");
                continue;
            }

            int size = strlen(input_data);

            struct data_t *data = data_create2(size, input_data);
            struct entry_t *entry = entry_create(key, data);

            rtree_put(tree, entry);
            printf("-----------------------\n");
        }

        else if(strcmp(token, "get") == 0){
            char *key = strtok(NULL, " \n");
            printf("KEY %s", key);
            printf("KEY %s\n", key);
            if(key == NULL){
                printf("O comando get tem de incluir uma chave!\n");
                printf("Exemplo: get <key>\n");
                continue;
            }

            struct data_t *data = rtree_get(tree, key);
            //fazer alguma coisa com a data
            data_destroy(data);
            printf("-----------------------\n");
        }

        else if(strcmp(token, "del") == 0){
            char *key = strtok(NULL, " \n");
            if(key == NULL){
                printf("O comando del tem de incluir uma chave!\n");
                printf("Exemplo: del <key>\n");
                continue;
            }

            rtree_del(tree, key);
            printf("-----------------------\n");
        }

        else if(strcmp(token, "size") == 0){
            printf("A arvore tem %d entradas\n", rtree_size(tree));
            printf("-----------------------\n");
        }

        else if(strcmp(token, "height") == 0){
            rtree_height(tree);
            printf("-----------------------\n");
        }

        else if(strcmp(token, "getkeys") == 0){
            char **keys = rtree_get_keys(tree);
            if(keys == NULL){
                printf("Ocorreu um erro a obter as chaves!\n");
                continue;
            }
            else{
                int tree_size =  rtree_size(tree);
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
            char **values = (char **) rtree_get_values(tree);
            if(values == NULL){
                printf("Ocorreu um erro a obter os valores!\n");
                continue;
            }
            else{
                int tree_size =  rtree_size(tree);
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

        else if(strcmp(token, "quit") == 0){
            printf("A terminar a execucao...\n");
            if(rtree_disconnect(tree) == -1){
                printf("Ocorreu um erro a desconectar!\n");
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