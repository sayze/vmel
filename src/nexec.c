#include <stdio.h>
#include <stdlib.h>
#include "nexec.h"
#include "utils.h"

#define ERR_UNDEFINE_VAR 0

static const char *Error_Templates[] = {
	"Use of undefined variable '$@0' near @1"
};

// Execute a string node.
static char *exec_string(Node *node) {
	return node->value;
}

// Get the value of a variable stored in symbol table.
// Return NULL if it doesn't exist or undefined.
static char *expand_variable(SyTable *sy_table, char *name) {
	if (!name)
		return NULL;
		
	Symbol *sy = SyTable_get_symbol(sy_table, name);
	
	if (!sy)
		return NULL;
		
	return sy->val;
}

// Expand a mixed string and return malloc'ed value. 
static char *exec_mixed_string(char *mstr, NexecMgr *nexec_mgr) {
	VString_set(&nexec_mgr->buff, mstr);

	char *m_str_it = strchr(mstr, VAR);

	// Proceed only if it has substitute char.
	if (m_str_it) {

		// Temp buffer for var names.
		VString buf = VString_new();
		// Expanded variable.
		char *var_val;

		while(m_str_it) {

			// Add '$' to buffer and increment.	
			VString_pushc(&buf, *m_str_it++);
		
			while(is_valid_identifier(*m_str_it)) {
				VString_pushc(&buf, *m_str_it);
				m_str_it++;
			}

			var_val = expand_variable(nexec_mgr->sy_table, buf.str+1);
			
			// Only replace if valid variable.
			if (!var_val) {
				NexecMgr_add_error(nexec_mgr->err_handle, buf.str+1, nexec_mgr->curr_node->value);
			}
			else {
				VString_replace(&nexec_mgr->buff, buf.str, var_val);
			}

			VString_set(&buf, "");
			m_str_it = strchr(m_str_it, VAR);
		}
		VString_free(&buf);
	}
	return nexec_mgr->buff.str;
}

// Execute a expression node (3 + 4).
static int exec_expression(NexecMgr *nexec_mgr, Node *node) {
	int ret = 0;
	Symbol *sy;

	switch(node->type) {
		case E_GREATERTHANEQ_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) >= exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_GREATERTHAN_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) > exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_LESSTHANEQ_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) <= exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_LESSTHAN_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) < exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_NEQUAL_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) != exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_EEQUAL_NODE:
			ret = exec_expression(nexec_mgr, node->data->BinExpNode.left) == exec_expression(nexec_mgr, node->data->BinExpNode.right);
			break;
		case E_ADD_NODE: 
			ret += exec_expression(nexec_mgr, node->data->BinExpNode.left) + exec_expression(nexec_mgr, node->data->BinExpNode.right);
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
		case E_INTEGER_NODE:
			ret = string_to_int(node->value, strlen(node->value));
			break;
		case E_STRING_NODE:
			ret = string_to_ascii(node->value);
			break;
		case E_MIXSTR_NODE:
			exec_mixed_string(node->value, nexec_mgr);
			ret = string_to_ascii(nexec_mgr->buff.str);
			break;
		case E_IDENTIFIER_NODE:
			sy = SyTable_get_symbol(nexec_mgr->sy_table, node->value);
			// TODO: At the moment no way of telling if identifier node
			// is a 'Number' string or 'Alpha	' string so we attempt to
			// first convert to integer if fails then fallback to ascii encoding.
			// A fix would be to include type information in the symbol table by
			// deducing all identifiers prior to function execution. But is this double 
			// handling ?
			ret = string_to_int(sy->val, strlen(sy->val));
			if (ret < 0) ret = string_to_ascii(sy->val);
			break;
		default:
			break;
	}
	return ret;
}

// Helper to convert intger to malloc'ed string.
static char *expr_to_string(NexecMgr *nexec_mgr, int src) {
	char *dest = malloc(6 * sizeof(char));
	char *out = NULL;
	
	// Store any additional bytes if needed.
	int extra_b = snprintf(dest, 6, "%d" , src);
	
	if (extra_b > 0) {
		out = realloc(dest, (extra_b + 6) * sizeof(char));
		dest = out;
	}

	VString_set(&nexec_mgr->buff, dest);
	free (dest);
	return nexec_mgr->buff.str;
}

void NexecMgr_add_error(Error *err_handle, char *offender, char *hint) {
	if (!err_handle || !offender)
		return;
	
	if (err_handle->error_cap == err_handle->error_ctr)
		return;
	
	// The error template which is needed.
	const char *template = Error_Templates[0];
	// Array containing string of substitute values.
	char *template_values[] = {offender, hint};
	// Final template error.
	char *template_fmt = NULL;

	template_fmt = string_map_vars(template, template_values, strlen(template), 2);

	err_handle->errors[err_handle->error_ctr] = template_fmt;
	err_handle->error_ctr++;
}	

NexecMgr *NexecMgr_new(void) {
	NexecMgr *n = malloc(sizeof(NexecMgr));
	n->err_handle = NULL;
	n->node_mgr = NULL;
	n->scope = 0;
	n->sy_table = NULL;
	n->curr_node = NULL;
	return n;
}

int NexecMgr_free(NexecMgr *nexec_mgr) {
	if (null_check(nexec_mgr, "nexecmgr free")) return -1;
	VString_free(&nexec_mgr->buff);
	free(nexec_mgr);
	return 0;
}

int Nexec_func_node(NexecMgr *nexec_mgr) {
	if (null_check(nexec_mgr, "nexec func node")) return -1;

	// Current node in manager.
	Node *curr_node = nexec_mgr->curr_node;
	// Pointer to function arguments.
	Node *curr_args = curr_node->data->FuncNode.args;
		
	if (string_compare(curr_node->value, "print")) {
		
		// Result of arithmetic operations.
		int calc = 0;

		switch (curr_args->type) {
			case E_STRING_NODE:
			case E_INTEGER_NODE:
				printf("%s\n", exec_string(curr_args));
				break;
			case E_IDENTIFIER_NODE:
				VString_set(&nexec_mgr->buff, expand_variable(nexec_mgr->sy_table, curr_args->value));
				if (nexec_mgr->buff.str)
					printf("%s\n", nexec_mgr->buff.str);
				else
					NexecMgr_add_error(nexec_mgr->err_handle, curr_args->value, curr_node->value);
				break;
			case E_MIXSTR_NODE:
				VString_set(&nexec_mgr->buff, exec_mixed_string(curr_args->value, nexec_mgr));
				printf("%s\n", nexec_mgr->buff.str);
				break;
			default:
				// Derive final value from operation node.
				calc = exec_expression(nexec_mgr, curr_args);
				printf("%d\n", calc);
				break;
		} 
	}
	return 0;
}

int Nexec_group_node(NexecMgr *nexec_mgr) {
	if (null_check(nexec_mgr, "nexec group node")) return -1;
	return 0;
}

int Nexec_assignment_node(NexecMgr *nexec_mgr) {
	if (null_check(nexec_mgr, "nexec assignment node")) return -1;

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
		VString_set(&nexec_mgr->buff, expand_variable(nexec_mgr->sy_table, asn_right_node->value));
		if (nexec_mgr->buff.str)
			SyTable_update_symbol(nexec_mgr->sy_table, asn_left_node->value, nexec_mgr->buff.str);
	}
	//TODO: Since no concept of ternary operators we can group storage of below.
	else if (Node_is_binop(asn_right_node)|| Node_is_compare(asn_right_node)) {
		
		// Derive final value from operation node.
		int calc = exec_expression(nexec_mgr, asn_right_node); 
		// Convert the integer to string.
		expr_to_string(nexec_mgr, calc);
		SyTable_update_symbol(nexec_mgr->sy_table, asn_left_node->value, nexec_mgr->buff.str);
	}
	else if (asn_right_node->type == E_MIXSTR_NODE) {
		exec_mixed_string(asn_right_node->value, nexec_mgr);
		SyTable_update_symbol(nexec_mgr->sy_table, asn_left_node->value, nexec_mgr->buff.str);
	}

	return 0;
}

NexecMgr *Nexec_init(SyTable *sy_table, NodeMgr *node_mgr, Error *err_handle) {
	if (null_check(sy_table, "nexec init") || null_check(node_mgr, "nexec init")) return NULL;

	// Setup wrapper structs.
	NexecMgr *nexec_mgr = NexecMgr_new();
	nexec_mgr->node_mgr = node_mgr;
	nexec_mgr->sy_table = sy_table;
	nexec_mgr->err_handle = err_handle;
	nexec_mgr->buff = VString_new();

	return nexec_mgr;
}

int Nexec_exec(NexecMgr *nexec_mgr, Node *node) {
	if (null_check(node ,"nexec exec") || null_check(node ,"nexec exec")) return -1;
	
	nexec_mgr->curr_node = node;
	switch (node->type) {
			case E_FUNC_NODE:
				Nexec_func_node(nexec_mgr);
				break;
			case E_EQUAL_NODE:
				Nexec_assignment_node(nexec_mgr);
				break;
			default:
				break;
	}
	
	Error_print_all(nexec_mgr->err_handle);
	return 0;
}
