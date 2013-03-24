#ifndef CBLIA_LILL_TREE_NODE
#define CBLIA_LILL_TREE_NODE

#include "lill_token.h"

#define LILL_NODE_MAX_CHILDREN 4

struct lill_tree_node {
	struct lill_tree_node *parent;
	struct lill_tree_node *children[LILL_NODE_MAX_CHILDREN];
	struct lill_token *token;
};

#endif /* CBLIA_LILL_TREE_NODE */
