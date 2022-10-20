// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <data.h>
#include <entry.h>
#include <serialization.h>
#include <stdlib.h>
#include <string.h>

/* Serializa todas as keys presentes no array de strings keys
 * para o buffer keys_buf que será alocado dentro da função.
 * O array de keys a passar em argumento pode ser obtido através 
 * da função tree_get_keys. Para além disso, retorna o tamanho do
 * buffer alocado ou -1 em caso de erro.
 */
int keyArray_to_buffer(char **keys, char **keys_buf){
    if(keys == NULL){
        return -1;
    }

    int size,i = 0;
    while(keys[i] != NULL){
        size ++; 
    }
    
    keys_buf = malloc(sizeof(char*) * size);
    while(keys[i] != NULL){
        keys_buf[i] = malloc(sizeof(char) * (strlen(keys[i]) + 1));
        memcpy(keys_buf[i], keys[i], strlen(keys[i]));
    }
    return size;
}

/* De-serializa a mensagem contida em keys_buf, com tamanho
 * keys_buf_size, colocando-a e retornando-a num array char**,
 * cujo espaco em memória deve ser reservado. Devolve NULL
 * em caso de erro.
 */
char** buffer_to_keyArray(char *keys_buf, int keys_buf_size){
    if(keys_buf == NULL || keys_buf_size <= 0){
        return NULL;
    }

    char **keys = malloc(sizeof(char*) * keys_buf_size);
    memcpy(keys, keys_buf, keys_buf_size);
    free(keys_buf);
    return keys;
}