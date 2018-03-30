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

int NodeMgr_free(NodeMgr *node_mgr) {
    if (node_mgr == NULL)
        return 1;

    struct Node *root_node = NULL;     
    struct Node *itr = NULL;
    struct Node *prev = NULL;
    unsigned int depth_ct = 1;
    
    for (size_t n = 0; n < node_mgr->nodes_ctr; n++) {
        root_node = node_mgr->nodes[n];
        switch (root_node->type) {
            /** TODO: At the moment only operator nodes contain left, right pointers
             * this means we need to use prev Node* to keep track of the last operator ast.
             * Would be less code and more generic if each node implemented a doubly linked list.
             * Also we can have a tree walker function that returns an array of 3 pointers to
             * ast at depth (x). Assuming an ast can't have more than 3 branches.
             */
            case E_EQUAL_NODE:
                // Depth 1 e.g 2 + 3       
                if (root_node->depth == 1) {
                    free(root_node->data->AsnStmtNode.left);
                    free(root_node->data->AsnStmtNode.right);
                }
                else {
                    free(root_node->data->AsnStmtNode.left);
                    itr = root_node->data->AsnStmtNode.right; 
                    while (depth_ct < root_node->depth) {
                        prev = itr;
                        free(itr->data->BinExpNode.left);
                        itr = itr->data->BinExpNode.right;
                        free(prev->data);
                        free(prev);
                        depth_ct++;
                    }
                    free(itr);
                    prev = NULL;
                    itr = NULL;
                }
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

    if (node_mgr->nodes == NULL)
        return 1;
    
    node_mgr->nodes[node_mgr->nodes_ctr++] = node;
    return 0;
}
