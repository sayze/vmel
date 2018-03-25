#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define INIT_NODEMGR_SIZE 100

NodeMgr *NodeMgr_new(void) {
    NodeMgr *node_mgr = malloc(sizeof(NodeMgr)) ;  
    node_mgr->nodes_ctr = 0;
    node_mgr->nodes_cap = INIT_NODEMGR_SIZE;
    node_mgr->nodes = malloc(node_mgr->nodes_cap * sizeof(struct Node*));
    return node_mgr;
}

int NodeMgr_free(NodeMgr *node_mgr) {
    if (node_mgr == NULL)
        return 1;
    
    // TODO: Need smarter way of freeing memory.
    // Existing solution makes too many assumptions.
    for (size_t n =0; n < node_mgr->nodes_ctr; n++) {
		struct Node *rn = node_mgr->nodes[n];
		if (strcmp(rn->value, "=") == 0) {
			free(rn->left);
			free(rn->right);
		}
		else {
			struct Node *itr = rn->right;
			while (strncmp(itr->value, "EOF", 3) != 0) {
				itr = itr->right;
				free(itr->left);
			}
			free(itr);
		}

		// Free root node.
		free(rn);
    }

    free(node_mgr);
    return 0;
}

struct Node *Node_new(void) {
    struct Node *n = malloc(sizeof(struct Node));
    n->left = NULL;
	n->right = NULL;
	n->value = NULL;
	return n;
}

struct Node **grow_nodes(NodeMgr *node_mgr) {
    if (node_mgr == NULL)
		return NULL;
		
	node_mgr->nodes_cap *= 2;
	struct Node **nodes_new = realloc(node_mgr->nodes, sizeof(struct Node *) * node_mgr->nodes_cap);		
	return nodes_new;
}

int NodeMgr_add_node(NodeMgr *node_mgr, struct Node *node) {
    if (node_mgr == NULL)
        return 1;

    if (node_mgr->nodes_cap - node_mgr->nodes_ctr == 4)
        node_mgr->nodes = grow_nodes(node_mgr);   

        if (node_mgr->nodes == NULL)
            return 1;
    
    node_mgr->nodes[node_mgr->nodes_ctr++] = node;
    return 0;
}
