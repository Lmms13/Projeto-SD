#include <network_server.h>
#include <tree_skel.h>

int main(int argc, char const *argv[]){
    if(argc != 2){
        printf("Incluir o numero do porto TCP ao qual o servidor se deve associar!\nExemplo de utilizacao: ./bin/tree-server <port>\n");
                return -1;
    }

    int socket = network_server_init(atoi(argv[1]));
    if(socket == -1){
        printf("Ocorreu um erro\n");
        return -1;
    }

    if(tree_skel_init() == -1){
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
