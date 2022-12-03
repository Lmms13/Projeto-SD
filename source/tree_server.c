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
    if(argc != 2){
        printf("Incluir o IP e o numero da porta TCP do Zookeeper!\n");
        printf("Exemplo de utilizacao: ./bin/tree-server <IP>:<porta>\n");
        return -1;
    }

    char *address = strtok((char*) argv[1], ":");
    int port = atoi(strtok(NULL, ":"));

    int socket = network_server_init(port);
    if(socket == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    if(network_zookeeper_init(address, port) == -1){
        printf("Ocorreu um erro a iniciar o zookeeper!\n");
        return -1;
    }

    int N = 1;
    if(tree_skel_init(N) == -1){
        printf("Ocorreu um erro\n");
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
