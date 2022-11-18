// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include "tree_skel.h"
#include <message-private.h>
#include <network_server.h>
#include <sdmessage.pb-c.h>
#include <network-private.h>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>

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

void network_abort(int n){
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
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, network_abort);
    printf("A espera de um cliente...\n");

    struct sockaddr_in client;
    socklen_t client_size;
    int socket, connected, value, result;
    socket = 0;

    int set_size = 50;  // valor arbitrário, o objetivo é ser superior ao necessário
    int desc_num = 1; //numero real de descriptors, tem de ser menor do que set_size 
    struct pollfd desc_set[set_size];
    struct pollfd cleanup_set[set_size]; //para limpar os valores -1 em desc_set


    for(int i = 1; i < set_size; i++){
        desc_set[i].fd = -1;
    }

    desc_set[0].fd = listening_socket;
    desc_set[0].events = POLLIN;

    while(poll(desc_set, desc_num, 50) >= 0){
        if((desc_num < set_size) && (desc_set[0].revents & POLLIN)){
            if(desc_set[desc_num].fd = accept(desc_set[0].fd, (struct sockaddr*) &client, &client_size) != -1){
                desc_set[desc_num].events = POLLIN;
                desc_num++;
                printf("Um cliente foi conectado!\n");
            }
        }

        for(int i = 1; i < desc_num; i++){
            if(desc_set[i].revents & POLLIN){
                struct message_t *msg = network_receive(socket);

                if(msg == NULL){
                    printf("A conexao com o cliente foi terminada!\n");
                    close(desc_set[i].fd);//este pode dar erro
                    desc_set[i].fd = -1;
                    continue;
                }
                else{
                    result = invoke(msg);
                    if(result == 0){
                        value = network_send(desc_set[i].fd, msg);
                        if(value == -1){
                            close(desc_set[i].fd);
                            return -1;//talvez não seja preciso dar return aqui
                        }
                    }
                }
            }
            if(desc_set[i].revents & POLLHUP){
                close(desc_set[i].fd);
                desc_set[i].fd = -1;
            }
        }
        //limpeza dos valores -1 de desc_set através de um array auxiliar
        for(int i = 1; i < desc_num; i++){
            for(int j = 1; j < desc_num; j++){
                if(desc_set[j].fd == -1){
                    cleanup_set[i].fd = desc_set[j].fd; 
                    desc_set[j].fd = -1;
                    break;
                }  
            }
        }
        for(int i = 1; i < desc_num; i++){
            desc_set[i].fd = cleanup_set[i].fd;
            if(cleanup_set[i].fd == -1 && cleanup_set[i-1].fd != -1){
                desc_num = i-1;
            }
        }
    }
    close(listening_socket);
    return 0;

    // while(1){
    //     printf("Sem resposta...\n"); //print para testar se o accept funcionou
    //     if((socket = accept(listening_socket, (struct sockaddr*) &client, &client_size)) != -1){
    //         printf("Um cliente foi conectado!\n");
    //         connected = 1;
    //         while(connected){
    //             struct message_t *msg = network_receive(socket);

    //             if(msg == NULL){
    //                 printf("A conexao com o cliente foi terminada!\n");
    //                 connected = 0;
    //                 // close(socket);
    //                 continue;
    //             }

    //             result = invoke(msg);
    //             if(result == 0){
    //                 value = network_send(socket, msg);
    //                 if(value == -1){
    //                     close(socket);
    //                     continue;
    //                 }
    //             }
    //             else{
    //                 continue;
    //             }
    //         }
    //     } 
    // }
    // return 0;
}



/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
    int nbytes, size_net_ord, size;

    size_net_ord = 0;
    nbytes = message_read_all(client_socket, &size_net_ord, sizeof(int));
    if(nbytes == -1){
        perror("Cliente fechou o socket!");
        close(client_socket);
        return NULL;
    };

    size = ntohl(size_net_ord);
    uint8_t *buffer = malloc(size);
    nbytes = message_read_all(client_socket, buffer, size);
    if(nbytes == -1){
        perror("O cliente fechou o socket");
        close(client_socket);
        return NULL;
    }

    struct message_t *msg = message_create();

    //teste para NULL -- close socket -- return NULL
    msg->content = *message_t__unpack(NULL, size, buffer);
    
    return msg;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg){
    int size = message_t__get_packed_size(&(msg->content));
    int size_net_ord = htonl(size);
    uint8_t *buffer = malloc(size);
    if(buffer == NULL){
        perror("Erro no malloc do buf!");
        return -1;
    }
    message_t__pack(&(msg->content), buffer);

    free(msg);
    
    int nbytes = message_write_all(client_socket, &size_net_ord, sizeof(int));
    if(nbytes != sizeof(int)){
        perror("Erro ao enviar dados ao cliente!");
        close(client_socket);
        return -1;
    }

    nbytes = message_write_all(client_socket, buffer, size);
    if(nbytes < 0){
        perror("Erro ao enviar todos os dados ao cliente!");
        close(client_socket);
        return -1;
    }

    free(buffer);
    return 0;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close(){
    tree_skel_destroy();
    return close(socket_num);
}
