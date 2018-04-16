#include <stdio.h>
#include <stdlib.h>
#include "nexec.h"
#include "utils.h"

NexecMgr *NexecMgr_new(void) {
	NexecMgr *n = malloc(sizeof(NexecMgr));
	n->err_handle = NULL;
	n->node_mgr = NULL;
	n->scope = 0;
	n->sy_table = NULL;
	n->curr_node = NULL;
	return n;
}

// Execute a string node.
static char *exec_string(Node *node) {
	return node->value;
}

// Get the value of a variable stored in symbol table.
// Return NULL if it doesn't exist or undefined.
static char *expand_variable(SyTable *sy_table, char *name) {
	Symbol *sy = SyTable_get_symbol(sy_table, name);
	
	if (!sy)
		return NULL;
		
	return sy->val;
}

// Execute a expression node (3 + 4).
static int exec_expression(NexecMgr *nexec_mgr, Node *node) {
	int ret = 0;
	
	switch(node->type) {
		case E_ADD_NODE:
			ret += exec_expression(nexec_mgr, node->data->BinExpNode.left) +  exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_MINUS_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) - exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_DIV_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) / exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_TIMES_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) *  exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;	
		default:
			if (node->type == E_IDENTIFIER_NODE) {
				Symbol *sy = SyTable_get_symbol(nexec_mgr->sy_table, node->value);
				ret = string_to_int(sy->val, strlen(sy->val));
			}
			else {
				ret = string_to_int(node->value, strlen(node->value));
			}
			break;
	}

	return ret;
}

// Helper to convert intger to malloc'ed string.
static char *expr_to_string(int src) {
	char *dest = malloc(6 * sizeof(char));
	char *out = NULL;
	
	// Store any additional bytes if needed.
	int extra_b = snprintf(dest, 6, "%d" , src);
	
	if (extra_b > 0) {
		out = realloc(dest, (extra_b + 6) * sizeof(char));
		dest = out;
	}

	return dest;
}

int NexecMgr_free(NexecMgr *nexec_mgr) {
	if (!nexec_mgr) {
		null_check("nexecmgr free");
		return -1;
	}

	free(nexec_mgr);
	return 0;
}

int Nexec_func_node(NexecMgr *nexec_mgr) {
	if (!nexec_mgr) {
		null_check("nexec fun node");
		return -1;
	}
	
	char *exp_var = NULL;
	int calc = 0;

	if (string_compare(nexec_mgr->curr_node->value, "print")) {
		switch (nexec_mgr->curr_node->data->CmpStmtNode.args->type) {
			case E_STRING_NODE:
			case E_INTEGER_NODE:
				printf("%s\n", exec_string(nexec_mgr->curr_node->data->CmpStmtNode.args));
				break;
			case E_IDENTIFIER_NODE:
				exp_var = expand_variable(nexec_mgr->sy_table, nexec_mgr->curr_node->data->CmpStmtNode.args->value);
				if (exp_var)
					printf("%s\n", exp_var);
				else
					printf("Error: could not access undefined variable '$%s'\n", nexec_mgr->curr_node->data->CmpStmtNode.args->value);
				break;
			default:
				// Derive final value from operation node.
				calc = exec_expression(nexec_mgr, nexec_mgr->curr_node->data->CmpStmtNode.args);
				printf("%d\n", calc);
				break;
		} 
	}

	return 0;
}

int Nexec_group_node(NexecMgr *nexec_mgr) {
	if (!nexec_mgr) {
		null_check("nexec group node");
		return -1;
	}

	return 0;
}

int Nexec_assignment_node(NexecMgr *nexec_mgr) {
	if (!nexec_mgr) {
		null_check("nexec assignment node");
		return -1;
	}

	// Left child node of assignment node.
	Node *asn_left_node = nexec_mgr->curr_node->data->AsnStmtNode.left;
	// Right child node of assignment node.
	Node *asn_right_node = nexec_mgr->curr_node->data->AsnStmtNode.right;
	
	// Determine which execution path to take based on the right side of assignment.
	if (asn_right_node->type == E_INTEGER_NODE || asn_right_node->type == E_STRING_NODE) {
		
		// Simple strings and integers just update the symbol value.
		SyTable_update_symbol(nexec_mgr->sy_table, asn_left_node->value, asn_right_node->value);
	}
	else if (asn_right_node->type == E_IDENTIFIER_NODE) {	
		// First expand variable value from symbol table.
		char *exp_val = expand_variable(nexec_mgr->sy_table, asn_right_node->value);
		if (exp_val)
			SyTable_update_symbol(nexec_mgr->sy_table, asn_left_node->value, exp_val);

	}
	else if (asn_right_node->type == E_ADD_NODE || asn_right_node->type == E_MINUS_NODE 
		|| asn_right_node->type == E_DIV_NODE || asn_right_node->type == E_TIMES_NODE) {
		
		// Derive final value from operation node.
		int calc = exec_expression(nexec_mgr, asn_right_node); 
		// Convert the integer to string.
		char *conv = expr_to_string(calc);
		
		SyTable_update_symbol(nexec_mgr->sy_table, asn_left_node->value, conv);
		
		// free original converted string.
		free(conv);
	}
	// else if (asn_right_node->type == E_MIXSTR_NODE) {
	// 	//TODO: Improve valid_idn function so that 
	// 	// it checks to see if its dollar sign then returns
	// 	// number of chars needing to be read.
	// 	size_t pos = 0;
	// 	char *var_full = string_find_vars(asn_right_node->value, VAR, &pos); 
	// }

	return 0;
}

NexecMgr *Nexec_init(SyTable *sy_table, NodeMgr *node_mgr) {
	if (!sy_table || !node_mgr) {
		null_check("nexec init");
		return NULL;
	}

	// Setup wrapper structs.
	NexecMgr *nexec_mgr = NexecMgr_new();
	nexec_mgr->node_mgr = node_mgr;
	nexec_mgr->sy_table = sy_table;

	return nexec_mgr;
}

int Nexec_exec(NexecMgr *nexec_mgr, Node *node) {
	if (!nexec_mgr || !node) {
		null_check("nexec exec");
		return -1;
	}
	
	nexec_mgr->curr_node = node;
	switch (node->type) {
			case E_CMPSTMT_NODE:
				Nexec_func_node(nexec_mgr);
				break;
			case E_EQUAL_NODE:
				Nexec_assignment_node(nexec_mgr);
				break;
			default:
				break;
	}

	return 0;
}
