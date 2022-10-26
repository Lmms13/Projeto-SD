#include "tree_skel.h"
#include <message-private.h>
#include <network_server.h>
#include <sdmessage.pb-c.h>
#include <network-private.h>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

struct sockaddr_in server;
int socket_num;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){
    signal(SIGINT, network_abort);

    // Cria socket TCP
    socket_num = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_num < 0 ) {
        perror("Erro ao criar socket");
        return -1;
    }

    // Preenche estrutura server para bind
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    int stay_port = 1;
    setsockopt(socket_num, SOL_SOCKET, SO_REUSEADDR, &stay_port, sizeof(int));
    if(bind(socket_num, (struct sockaddr*) &server, sizeof(server)) < 0){
        perror("Erro ao fazer bind");
        close(socket_num);
        return -1;
    };

    // Faz listen
    if (listen(socket_num, 0) < 0){
        perror("Erro ao executar listen");
        close(socket_num);
        return -1;
    };

    return socket_num;
}

network_abort(int n){
    network_server_close();
    printf("Ctrl+C detetado. A encerrar o servidor...\n");
    exit(1);
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){
    signal(SIGINT, network_abort);
    printf("A espera de um cliente...\n");

    struct sockaddr_in client;
    socklen_t client_size;
    int socket, connected, result;
    while(1){

        if((socket = accept(listening_socket, (struct sockaddr*) &client, &client_size)) != -1){
            printf("Um cliente foi conectado!\n");
            connected = 1;
            while(connected){
                struct message_t*msg = network_receive(socket);

                if(msg == NULL){
                    printf("A conexao com o cliente foi terminada!\n");
                    connected = 0;
                    close(socket);
                    continue;
                }

                invoke(msg);
                result = network_send(socket, msg);
                if(result == -1){
                    close(socket);
                        return -1;
                }
            }
        } 
    }
    return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){

    int nbytes, size_net_ord, size;

    int size_net_ord = 0;
    nbytes = read_all(client_socket, &size_net_ord, sizeof(int));
    if(nbytes == 0){
        perror("Erro ao receber dados do cliente!");
        close(client_socket);
        return NULL;
    };

    size = ntohl(size_net_ord);
    uint8_t *buffer = malloc(size);
    nbytes = read_all(client_socket, buffer, size);
    if(nbytes < 0){
        perror("Erro ao receber todos os dados do servidor");
        close(client_socket);
        return NULL;
    }

    struct message_t *msg = message_create();
    msg->content = *message_t__unpack(NULL, size, buffer);
    
    if(msg->content.opcode == MESSAGE_T__OPCODE__OP_ERROR){
        return NULL;
    }
    else{
        return msg;
    }
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg){
    int size = message_t__get_packed_size(&msg->content);
    int size_net_ord = htonl(size);
    uint8_t *buffer = malloc(size);
    if(buffer == NULL){
        perror("Erro no malloc do buf!");
        return NULL;
    }
    message_t__pack(&msg->content, buffer);

    free(msg);
    
    int nbytes = write_all(client_socket, &size_net_ord, sizeof(int));
    if(nbytes != sizeof(int)){
        perror("Erro ao enviar dados ao cliente!");
        close(client_socket);
        return NULL;
    }

    nbytes = write_all(client_socket, buffer, size);
    if(nbytes < 0){
        perror("Erro ao enviar todos os dados ao cliente!");
        close(client_socket);
        return NULL;
    }

    free(buffer);
    return 0;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close(){
    tree_skel_destroy();
    close(socket_num);
    return 0;
}
