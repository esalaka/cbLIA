#ifndef CBLIA_DESCENT_H
#define CBLIA_DESCENT_H

/* Forward declarations */
struct lill_token;
struct lill_tree_node;
void free_tree_recursively(struct lill_tree_node *tree);

/* Functions */
int lill_descend(struct lill_token *token_str, int token_count, struct lill_tree_node **tree);

#endif /* CBLIA_DESCENT_H */
