// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <data.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Função que cria um novo elemento de dados data_t, reservando a memória
 * necessária para armazenar os dados, especificada pelo parâmetro size 
 */
struct data_t *data_create(int size){
    struct data_t *data = malloc(sizeof(struct data_t));

    if(data == NULL || size <= 0){
        return NULL;
    }

    data->datasize = size;
    data->data = malloc(size);

    if(data->data == NULL){
        return NULL;
    }

    return data;
}

/* Função que cria um novo elemento de dados data_t, inicializando o campo
 * data com o valor passado no parâmetro data, sem necessidade de reservar
 * memória para os dados.
 */
struct data_t *data_create2(int size, void *data){
    struct data_t *datastruct = malloc(sizeof(struct data_t));

    if(datastruct == NULL || size <= 0 || data == NULL){
        return NULL;
    }

    datastruct->datasize = size;
    datastruct->data = malloc(size);

    if(datastruct->data == NULL){
        return NULL;
    }

    datastruct->data = data;
    return datastruct;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){
    if(data != NULL && data->data != NULL){
        free(data->data);
        free(data);
    }
    else if(data != NULL && data->data == NULL){
        free(data);    
    }
}

/* Função que duplica uma estrutura data_t, reservando toda a memória
 * necessária para a nova estrutura, inclusivamente dados.
 */
struct data_t *data_dup(struct data_t *data){
    if(data == NULL || data->data == NULL || data->datasize <= 0){
        return NULL;
    }
    void* content = malloc(data->datasize);
    memcpy(content, data->data, data->datasize);
    return data_create2(data->datasize, content);
//     struct data_t *dup = malloc(sizeof(struct data_t));
//     dup->data = malloc(data->datasize);
//     dup->datasize = data->datasize;
//     dup->data = data->data;
//    // strcpy(dup->data, data->data);
//     return dup;
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
*  Deve assegurar que destroi o conteúdo antigo do mesmo.
*/
void data_replace(struct data_t *data, int new_size, void *new_data){
    if(data == NULL || new_size <= 0 || new_data == NULL){
        return;
    }
    else{
        free(data->data);
        data->datasize = new_size;
        data->data = new_data;
    }
}
