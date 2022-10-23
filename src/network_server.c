#include "tree_skel.h"
#include <message-private.h>
#include <network_server.h>
#include <sdmessage.pb-c.h>

struct sockaddr_in socket;
int socket_num;

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){

}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket);

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){

    int nbytes, size_net_ord, size;

    int size_net_ord = 0;
    //sizeof(int) ou constante MAX_MSG? 
    //descobre-se o size com o read e depois lê-se com o read all?
    //fecha-se o client socket?
    nbytes = read(client_socket, &size_net_ord, sizeof(int));
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
    buffer[size - 1] = '\0';

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
    uint8_t *buf = malloc(size);
    if(buf == NULL){
        perror("Erro no malloc do buf!");
        return NULL;
    }
    message_t__pack(&msg->content, buf);

    //free à mensagem faz-se assim? dando unpack ao que foi packed?
    message_t__free_unpacked(&msg, NULL);
    ////////////////////////
    
    int nbytes = write(client_socket, &size_net_ord, sizeof(int));
    if(nbytes != sizeof(int)){
        perror("Erro ao enviar dados ao cliente!");
        close(client_socket);
        return NULL;
    }

    nbytes = write_all(client_socket, buf, size);
    if(nbytes < 0){
        perror("Erro ao enviar todos os dados ao cliente!");
        close(client_socket);
        return NULL;
    }

    free(buf);
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
