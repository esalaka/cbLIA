#ifndef CBLIA_GENERATE_H
#define CBLIA_GENERATE_H

struct lill_tree_node;

struct lill_function_definition
{
    char name[64];
};

void generate_code(const char *file_stem, struct lill_tree_node *tree);

#endif /* CBLIA_GENERATE_H */
