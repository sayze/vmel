#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"
#include "utils.h"

#define INIT_NODEMGR_SIZE 100

NodeMgr *NodeMgr_new(void) {
	NodeMgr *node_mgr = malloc(sizeof(NodeMgr)) ;  
    node_mgr->nodes_ctr = 0;
    node_mgr->nodes_cap = INIT_NODEMGR_SIZE;
    node_mgr->nodes = malloc(node_mgr->nodes_cap * sizeof(Node *));
    return node_mgr;
}

static int is_binop_node(Node *n) {
	return 	n->type ==  E_ADD_NODE || n->type == E_MINUS_NODE || 
			n->type == E_DIV_NODE || n->type == E_TIMES_NODE;
}

static int is_array_node(Node *n) {
	return 	n->type ==  E_ARRAY_NODE;
}

static void node_free(Node *node) {
	if (!node) 
		return;

	if (is_array_node(node)) {
		for (size_t i = 0; i < node->data->ArrayNode.dctr; i++) {
			if (is_array_node(node->data->ArrayNode.items[i])) {
				node_free(node->data->ArrayNode.items[i]);	
			}
			else {
				free(node->data->ArrayNode.items[i]);
			}
		} 	
		free(node->data->ArrayNode.items);
	}

	if (is_binop_node(node))
		node_free(node->data->BinExpNode.left);
		
	if (is_binop_node(node))
		node_free(node->data->BinExpNode.right);
	
	if (is_binop_node(node) || is_array_node(node))
		free(node->data);
	
	free(node);
}

int NodeMgr_free(NodeMgr *node_mgr) {
    if (!node_mgr) 
		return -1;

    Node *root_node = NULL;     
    Node *itr = NULL;
    Node *prev = NULL;
    
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
			case E_CMPSTMT_NODE:
				node_free(root_node->data->CmpStmtNode.args);
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

Node *Node_new(int wdata) {
    Node *n = malloc(sizeof(struct Node));
    
    // Malloc data if needed.
    if (wdata)
        n->data = malloc(sizeof(union SyntaxNode));
    else
        n->data = NULL;
    
    n->depth = 0;
    n->type = E_EOF_NODE;
    return n;
}

Node **grow_nodes(NodeMgr *node_mgr) {
    if (!node_mgr) 
		return NULL;

    node_mgr->nodes_cap *= 2;
    Node **nodes_new = realloc(node_mgr->nodes, sizeof(Node *) * node_mgr->nodes_cap);		
    return nodes_new;
}

int NodeMgr_add_node(NodeMgr *node_mgr, Node *node) {
    if (!node_mgr) 
		return -1;

    if (node_mgr->nodes_cap - node_mgr->nodes_ctr == 4)
        node_mgr->nodes = grow_nodes(node_mgr);   

    if (!node_mgr->nodes) 
		return -1;
    
    node_mgr->nodes[node_mgr->nodes_ctr++] = node;
    return 0;
}
