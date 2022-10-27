
#include <stdio.h>
#include <string.h>
#include <client_stub.h>


#define BUFFERSIZE 10
int main(int argc, char const *argv[]){

    if(argc != 2){
        printf("Incluir o endereco IP ou nome do servidor da arvore e o numero do porto TCP aoqual o servidor esta a espera de ligacoes! \nExemplo de utilizacao: ./bin/tree-client<server>:<port>\n");
                return -1;
    }
    // Retirar valores de ip e numero de port
    const char[] *server_ip = argv[0];
    const char[] *port_str = argv[1];
    struct rtree_t *tree;
    tree = rtree_connect(port_str);
    if(tree == NULL){
        printf("Error\n\t Association between client and server led to an errorAssocia to server\n");
        return -1;
    }
    while(true){
        char buffer[BUFFERSIZE];
        printf("Enter a command: \n");
        fgets(buffer, BUFFERSIZE , stdin);
        const char space = " ";
        char *token;
   
        /* get the first token */
        token = strtok(buffer, space);
        /* walk through other tokens */
        while( token != NULL ) {
            printf( " %s\n", token );
        }   
        token = strtok(NULL, s);
                
        if(strcomp(token,"size") == 0){
            int size = rtree_size(tree);
            printf("Number of elements in the tree : %d \n", size);
        }else if(strcomp(token,"height")== 0){
            int height = rtree_height(tree);
            printf("The height of the tree is %d \n", height)
        }else if(strcomp(token,"del")== 0){
            int del = rtree_del(tree, );
            if(del == 0){
                printf("OK");
            }else if(del == -1){
                printf("ERROR \n\t Key not found or other problems\n")
            }
        }else if(strcomp(token,"get")== 0){
            struct data_t data = rtree_get(tree, );
            if(&data == NULL){
                printf("ERROR \n\t Error at getting element\n");
            }
        }else if(strcomp(token,"put")== 0){
            int put = rtree_put(tree);
            if(put == 0){
                printf("OK");
            }else if(put == -1){
                printf("ERROR\n\t Problems occurred\n");
            }
        }else if(strcomp(token,"getKeys")== 0){
            char* keys = rtree_get_keys(tree);
            printf("%s", keys);
        }else if(strcomp(token,"getvalues")== 0){
            rtree_get_values(tree);
        }else if(strcomp(token,"quit")== 0){
            int disconnect = rtree_disconnect(tree);
            if(disconnect == 0){
                printf("Client closing\n");
            }else if(disconnect == -1){
                printf("ERROR \n\t Error at disconnect\n");
            }      
            break;
        }else {
            printf("Command not recognized \n Try again\n");
        }
          
    }
    
    return 0;
    
}