// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <data.h>
#include <entry.h>

#include <string.h>
#include <stdlib.h>

/* Função que cria uma entry, reservando a memória necessária para a
 * estrutura e inicializando os campos key e value, respetivamente, com a
 * string e o bloco de dados passados como parâmetros, sem reservar
 * memória para estes campos.

 */
struct entry_t *entry_create(char *key, struct data_t *data){
    struct entry_t *entry = (struct entry_t *)malloc(sizeof(struct entry_t));
    if(entry == NULL){
        return NULL;
    }

    entry->key = key;
    entry->value = data;
    return entry;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
    if(entry != NULL && entry->value != NULL){
        data_destroy(entry->value);
       // free(entry->key);
        free(entry);
    }
    else if(entry != NULL && entry->value == NULL){
        free(entry);   
    }
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){
    if(entry == NULL){
        return NULL;
    }
    char* newkey = strdup(entry->key);
    return entry_create(newkey, data_dup(entry->value));
}

/* Função que substitui o conteúdo de uma entrada entry_t.
*  Deve assegurar que destroi o conteúdo antigo da mesma.
*/
void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    if(entry == NULL || new_key == NULL || new_value == NULL){
        return;
    }
    else{    
        free(entry->key);
        data_destroy(entry->value);
        entry->key = new_key;
        entry->value = new_value;
    }
}

/* Função que compara duas entradas e retorna a ordem das mesmas.
*  Ordem das entradas é definida pela ordem das suas chaves.
*  A função devolve 0 se forem iguais, -1 se entry1<entry2, e 1 caso contrário.
*/
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    int comp = strcmp(entry1->key, entry2->key);
    if (comp == 0){
        return 0;
    }
    else if (comp < 0){
        return -1;
    }
    else 
        return 1;
}
