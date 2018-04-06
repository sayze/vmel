#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"

#define INIT_NODEMGR_SIZE 100

NodeMgr *NodeMgr_new(void) {
	NodeMgr *node_mgr = malloc(sizeof(NodeMgr)) ;  
    node_mgr->nodes_ctr = 0;
    node_mgr->nodes_cap = INIT_NODEMGR_SIZE;
    node_mgr->nodes = malloc(node_mgr->nodes_cap * sizeof(struct Node*));
    return node_mgr;
}

static int is_binop_node(struct Node *n) {
	return 	n->type ==  E_ADD_NODE || n->type == E_MINUS_NODE || 
			n->type == E_DIV_NODE || n->type == E_TIMES_NODE;
}

static void node_free(struct Node *node) {
	if (node == NULL) 
		return;
	
	if (is_binop_node(node))
		node_free(node->data->BinExpNode.right);
		
	if (is_binop_node(node))
		node_free(node->data->BinExpNode.left);
	
	if (is_binop_node(node))
		free(node->data);
	
	free(node);
}

int NodeMgr_free(NodeMgr *node_mgr) {
    if (node_mgr == NULL) 
		return 1;

    struct Node *root_node = NULL;     
    struct Node *itr = NULL;
    struct Node *prev = NULL;
    
    for (size_t n = 0; n < node_mgr->nodes_ctr; n++) {
		root_node = node_mgr->nodes[n];
        switch (root_node->type) {
            case E_EQUAL_NODE:
				itr = root_node->data->AsnStmtNode.right;
				free(root_node->data->AsnStmtNode.left);
				node_free(itr);
                break;
            case E_GROUP_NODE:
                itr = root_node->data->GroupNode.next;
                while (itr != root_node) {
                    prev = itr;
                    itr = itr->data->GroupNode.next;
                    free(prev->data);
                    free(prev);
			    }
                break;
            default:
                break;
        }

        /**
         * Free root node of every AST.
         *             =   ---> free this node.
         *           /   \
         *          /     \
         *          s      +
         *                / \
         *               /   \
         *              1     3
         */               
        free(root_node->data);
        free(root_node);
    }

	free(node_mgr->nodes);
    free(node_mgr);
    return 0;
}

struct Node *Node_new(int wdata) {
    struct Node *n = malloc(sizeof(struct Node));
    
    // Malloc data if needed.
    if (wdata)
        n->data = malloc(sizeof(union SyntaxNode));
    else
        n->data = NULL;
    
    n->depth = 0;
    n->type = E_EOF_NODE;
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

    if (node_mgr->nodes == NULL) return 1;
    
    node_mgr->nodes[node_mgr->nodes_ctr++] = node;
    return 0;
}
