#include <string.h>

#include "lill_tree_node.h"
#include "function.h"
#include "generate.h"

void buildFunctionDefinitions(struct lill_tree_node *curr, struct lill_function_definition **defs)
{
    struct lill_function_definition *currdef;
    currdef = *defs;
    struct lill_tree_node *parent;

    if (curr->type != NODE_DEFINITIONS)
    {
        return; /* Error here */
    }
}

