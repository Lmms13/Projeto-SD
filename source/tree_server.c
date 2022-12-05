// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <network_server.h>
#include <network-private.h>
#include <tree_skel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]){
    if(argc != 3){
        printf("Incluir o IP e o numero da porta do servidor e do Zookeeper!\n");
        printf("Exemplo de utilizacao: ./bin/tree-server <IP>:<porta> <IP>:<porta>\n");
        return -1;
    }

    char* server_address_port = (char*) argv[1];
    char* zoo_address_port = (char*) argv[2];
    char *address = strtok((char*) argv[1], ":");
    int port = atoi(strtok(NULL, ":"));

    int socket = network_server_init(port);
    if(socket == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    int N = 1;
    if(tree_skel_init(N, server_address_port) == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    if(network_zookeeper_init(zoo_address_port) == -1){
        printf("Ocorreu um erro a iniciar o zookeeper!\n");
        return -1;
    }

    int result = network_main_loop(socket);
    if(result == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    if(network_server_close() == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    tree_skel_destroy();
    return 0;
}
