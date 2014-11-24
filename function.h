#ifndef CBLIA_FUNCTION_H
#define CBLIA_FUNCTION_H

struct lill_tree_node;
struct lill_function_definition;

void buildFunctionDefinitions(struct lill_tree_node *curr, struct lill_function_definition **defs);

#endif /* CBLIA_FUNCTION_H */
