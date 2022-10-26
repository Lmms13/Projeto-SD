#include <client_stub.h>
#include <client_stub-private.h>
#include <network_client.h>
#include <message-private.h>
#include <sdmessage.pb-c.h>
#include <network-private.h>

#include <signal.h>

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtree;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtree;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtree_t *rtree){
    if(rtree == NULL){
        return -1;
    }

    rtree->socket_num = socket(AF_INET, SOCK_STREAM, 0);
    if(rtree->socket_num < 0){
        perror("Erro ao criar socket TCP %d", rtree->socket_num);
        return -1;
    }

    if(connect(rtree->socket_num, (struct sockaddr *)&rtree->socket,
                sizeof(rtree->socket)) < 0){
        perror("Erro ao conectar-se ao servidor");
        network_close(rtree);
        return -1;
    }

    //posso colocar um print para comunicar o sucesso
    signal(SIGPIPE, network_pipe_close);
    return 0;
}

void network_pipe_close(int n){
    perror("Socket fechado de forma inesperada por uma das pontas!");
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtree_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtree_t * rtree, struct message_t *msg){
    if(rtree == NULL || msg == NULL){
        return NULL;
    }

    int descriptor = rtree->socket_num;
    int size = message_t__get_packed_size(&msg->content);
    int size_net_ord = htonl(size);
    uint8_t *buffer = malloc(size);
    if(buffer == NULL){
        perror("Erro no malloc do buf");
        return NULL;
    }
    message_t__pack(&msg->content, buffer);
    
    int nbytes = write_all(descriptor, &size_net_ord, sizeof(int));
    if(nbytes != sizeof(int)){
        perror("Erro ao enviar dados ao servidor");
        close(descriptor);
        return NULL;
    }

    nbytes = write_all(descriptor, buffer, size);
    if(nbytes < 0){
        perror("Erro ao enviar todos os dados ao servidor");
        close(descriptor);
        return NULL;
    }

    free(buffer);
    size_net_ord = 0;
    nbytes = read_all(descriptor, &size_net_ord, sizeof(int));
    if(nbytes != sizeof(int)){
        perror("Erro ao receber dados do servidor");
        close(descriptor);
        return NULL;
    };

    size = ntohl(size_net_ord);
    buffer = malloc(size);
    nbytes = read_all(descriptor, buffer, size);
    if(nbytes < 0){
        perror("Erro ao receber todos os dados do servidor");
        close(descriptor);
        return NULL;
    }
    // buffer[size - 1] = '\0';

    msg->content = *message_t__unpack(NULL, size, buffer);
    
    if(msg->content.opcode == MESSAGE_T__OPCODE__OP_ERROR){
        return NULL;
    }
    else{
        return msg;
    }
}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtree_t * rtree){
    return close(rtree->socket_num);
}
