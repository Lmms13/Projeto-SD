// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#include <tree.h>
#include <tree-private.h>
#include <entry.h>
#include <data.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Função para criar uma nova árvore tree vazia.
 * Em caso de erro retorna NULL.
 */
struct tree_t *tree_create(){
    struct tree_t *tree = malloc(sizeof(struct tree_t)); 

    if(tree == NULL){
        return NULL;
    }

    tree->entry = NULL;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

/* Função para libertar toda a memória ocupada por uma árvore.
 */
void tree_destroy(struct tree_t *tree){
    if(tree == NULL){
        return;
    }
    else{
        entry_destroy(tree->entry);
        tree_destroy(tree->left);
        tree_destroy(tree->right);
        free(tree);
    }
}

/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int tree_put(struct tree_t *tree, char *key, struct data_t *value){
    if(tree == NULL || key == NULL || value == NULL){
        return -1;
    }

    char *dup_key = malloc(strlen(key) + 1);
    strcpy(dup_key, key);
    struct entry_t *new_entry = entry_create(dup_key, data_dup(value));

    if(tree->entry == NULL){
        tree->entry = new_entry;
        return 0;
    }

    int comp = entry_compare(new_entry, tree->entry);

    if(comp == 0){
        entry_replace(tree->entry, new_entry->key, new_entry->value);
        return 0;
    }
    else if(comp == -1){
        if(tree->left == NULL){
            tree->left = tree_create();
        }
        entry_destroy(new_entry);
        return tree_put(tree->left, key, value);
    }
    else{
        if(tree->right == NULL){
            tree->right = tree_create();
        }
        entry_destroy(new_entry);
        return tree_put(tree->right, key, value);
    }
}

/* Função para obter da árvore o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou tree_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da árvore,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função. Devolve NULL em caso de erro.
 */
struct data_t *tree_get(struct tree_t *tree, char *key){
    if(tree == NULL || key == NULL || tree->entry == NULL){
        return NULL;
    }

    int comp = strcmp(key, tree->entry->key);

    if(comp == 0){
        struct data_t *value = data_dup(tree->entry->value);
        return value;
    }
    else if(comp < 0){
        return tree_get(tree->left, key);
    }
    else{
        return tree_get(tree->right, key);
    }
}

/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){
    if(tree == NULL || key == NULL || tree->entry == NULL){
        return -1;
    }
    int comp = strcmp(key, tree->entry->key);

    if(comp == 0){
        tree_destroy(tree);
        return 0;
    }
    
    else if(comp < 0){
        if(tree->left != NULL){
            if(strcmp(key, tree->left->entry->key) == 0){
                if(tree->left->right == NULL && tree->left->left == NULL){
                    printf("destroying %s\n", tree->left->entry->key);
                    tree_destroy(tree->left);
                    tree->left == NULL;
                }
                else{
                    struct tree_t* replacement_tree = tree_find_nearest(tree->left); 
                    tree_del_from_prev(tree->left, replacement_tree->entry->key); 
                    entry_destroy(tree->left->entry);
                    tree->left->entry = entry_dup(replacement_tree->entry);
                    printf("destroying %s\n", replacement_tree->entry->key);
                    tree_destroy(replacement_tree);

                }
                return 0;
            }
            else{
                return tree_del(tree->left, key);
            }
        }
        else{
            return -1;
        }
    }
    else{
        if(tree->right != NULL){
            if(strcmp(key, tree->right->entry->key) == 0){
                if(tree->right->right == NULL && tree->right->left == NULL){
                    printf("destroying %s\n", tree->right->entry->key);
                    tree_destroy(tree->right);
                    tree->right == NULL;
                }
                else{
                    struct tree_t* replacement_tree = tree_find_nearest(tree->right); 
                    tree_del_from_prev(tree->right, replacement_tree->entry->key); 
                    entry_destroy(tree->right->entry);
                    tree->right->entry = entry_dup(replacement_tree->entry);
                    printf("destroying %s\n", replacement_tree->entry->key);
                    tree_destroy(replacement_tree);
                }
                return 0;
            }
            else{
                return tree_del(tree->right, key);
            }
        }
        else{
            return -1;
        }
    }
}

int tree_del_from_prev(struct tree_t *tree, char* key){
    if(tree == NULL || key == NULL || tree->entry == NULL || tree->entry->key == NULL){
        return -1;
    }

    int comp = strcmp(key, tree->entry->key);
    int comp_r, comp_l = 999;
    if(tree->right != NULL){
        comp_r = strcmp(key, tree->right->entry->key);
    }

    if(tree->left != NULL){
        comp_l = strcmp(key, tree->left->entry->key);
    }
    if(comp_r == 0){
        tree->right = NULL;
        return 0;
    }
    else if(comp_l == 0){
        tree->left = NULL;
        return 0;
    }
    else if(comp < 0){
        return tree_del_from_prev(tree->left, key);
    }
    else{
        return tree_del_from_prev(tree->right, key);
    }
}

struct tree_t *tree_find_nearest(struct tree_t *tree){
    if(tree == NULL){
        return NULL;
    }

    if(tree->left == NULL && tree->right == NULL){
        return tree;
    }

    int l_height = tree_height(tree->left);
    int r_height = tree_height(tree->right);

    if(l_height <= r_height){
        if(tree->left == NULL){
            return tree_find_deepest(tree->right, -1);
        }
        else{
            return tree_find_deepest(tree->left, 1);
        }
    }
    else{
        if(tree->right == NULL){
            return tree_find_deepest(tree->left, 1);
        }
        else{
            return tree_find_deepest(tree->right, -1);
        }
    }
}

struct tree_t *tree_find_deepest(struct tree_t *tree, int dir){
    if(tree == NULL || (dir != -1 && dir != 1)){
        return NULL;
    }

    if(dir == -1){
        if(tree->left != NULL){
            return tree_find_deepest(tree->left, -1);
        }
        else{
            return tree;
        }
    }
    else{
        if(tree->right != NULL){
            return tree_find_deepest(tree->right, 1);
        }
        else{
            return tree;
        }
    }
}

/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){
    if(tree == NULL){
        return 0;
    }
    else if(tree->entry == NULL){
        return 0;
    }
    else{
        int left = tree_size(tree->left);
        int right = tree_size(tree->right);
        return left + 1 + right;
    }
}

/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){
    if(tree == NULL){
        return 0;
    }
    else{
        int left = tree_height(tree->left) + 1;
        int right = tree_height(tree->right) + 1;

        if(left > right){
            return left;
        }
        else{
            return right;
        }
    }
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária. As keys devem vir ordenadas segundo a ordenação lexicográfica das mesmas.
 */
char **tree_get_keys(struct tree_t *tree){
    if(tree == NULL){
        return NULL;
    }

    char** keys = malloc(sizeof(char*) * ((tree_size(tree) + 1)));

    if(keys == NULL){
        return NULL;
    }

    int n = 0;
    tree_get_keys_rec(tree, keys, &n);
    keys[n] = NULL;
    return keys;
}


void tree_get_keys_rec(struct tree_t *tree, char** keys, int* n){
    if(tree == NULL || keys == NULL){
        return;
    }

    tree_get_keys_rec(tree->left, keys, n);
    keys[*n] = malloc(sizeof(char*)); 
    keys[*n] = tree->entry->key;
    (*n)++;
    tree_get_keys_rec(tree->right, keys, n);
}

/* Função que devolve um array de void* com a cópia de todas os values da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
void **tree_get_values(struct tree_t *tree){
    if(tree == NULL){
        return NULL;
    }

    void** values = malloc(sizeof(struct data_t) * (tree_size(tree) + 1));
    if(values == NULL){
        return NULL;
    }

    int i = 0;
    char** keys = tree_get_keys(tree);

    while(keys[i] != NULL){
        values[i] = data_dup(tree_get(tree, keys[i]))->data;
        i++;
    }

    values[i] = NULL;
    return values;
}


/* Função que liberta toda a memória alocada por tree_get_keys().
 */
void tree_free_keys(char **keys){
    int i = 0;
    while(keys[i] != NULL){
        free(keys[i]);
        i++;
    }
}

/* Função que liberta toda a memória alocada por tree_get_values().
 */
void tree_free_values(void **values){
    int i = 0;
    while(values[i] != NULL){
        data_destroy(values[i]);
        i++;
    }
}
