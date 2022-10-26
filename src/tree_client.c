
#include <stdio.h>
#include <string.h>
#include <client_stub.h>

#define BUFFERSIZE 10
int main(int argc, char const *argv[]){
    if(argc != 2){
        printf("Incluir o endereco IP ou nome do servidor da arvore e o numero do porto TCP ao qual o servidor esta a espera de ligacoes!\nExemplo de utilizacao: ./bin/tree-client<server>:<port>\n");
                return -1;
    }
    // Retirar valores de ip e numero de port
    const char[] server_ip = argv[0];
    const char[] port_str = argv[1];
    rtree_connect(port_str);
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
            rtree_size();
        }else if(strcomp(token,"height")== 0){
            rtree_height();
        }else if(strcomp(token,"del")== 0){
            rtree_del();
        }else if(strcomp(token,"get")== 0){
            rtree_get();
        }else if(strcomp(token,"put")== 0){
            rtree_put();
        }else if(strcomp(token,"getKeys")== 0){
            rtree_get_keys();
        }else if(strcomp(token,"getvalues")== 0){
            rtree_get_values();
        }else if(strcomp(token,"quit")== 0){
            printf("Client closing");
            break;
        }else {
            printf("Command not recognized \n Try again\n")
        }
          
    }
    
    return 0;
    
}