#include <network_server.h>
#include <tree_skel.h>

int main(int argc, char const *argv[]){
    if(argc != 2){
        printf("Incluir o numero do porto TCP ao qual o servidor se deve associar!\nExemplo de utilizacao: ./bin/tree-server <port>\n");
                return -1;
    }

    int socket = network_server_init(atoi(argv[1]));
    tree_skel_init();
    int result = network_main_loop(socket);
    network_server_close();
    tree_skel_destroy();
    return 0;
}
