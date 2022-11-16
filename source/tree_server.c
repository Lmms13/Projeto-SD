// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <network_server.h>
#include <tree_skel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]){
    if(argc != 3){
        printf("Incluir o numero do porto TCP ao qual o servidor se deve associar e o numero de threads a criar!\n");
        printf("Exemplo de utilizacao: ./bin/tree-server <port> <N>\n");
        return -1;
    }

    int socket = network_server_init(atoi(argv[1]));
    if(socket == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    int N = atoi(argv[2]);
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
