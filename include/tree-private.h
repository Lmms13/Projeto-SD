// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "tree.h"
#include "entry.h"

struct tree_t {
	struct entry_t* entry;
	struct tree_t* left;
	struct tree_t* right;
};

void tree_get_keys_rec(struct tree_t *tree, char** keys, int* n);

struct tree_t *tree_find_nearest(struct tree_t *tree);

//-1 for left, 1 for right
struct tree_t *tree_find_deepest(struct tree_t *tree, int dir);

int tree_del_from_prev(struct tree_t *tree, char* key);

#endif
