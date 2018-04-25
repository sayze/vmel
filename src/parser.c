#include <stdio.h>
#include <stdlib.h>
#include "utils.h"	
#include "parser.h"
#include "tokenizer.h"   
#include "node.h"
#include "sytable.h"
#include "errors.h"

// Below are the errors which map to Error_Templates.
#define ERR_UNEXPECTED 0
#define ERR_GROUP_EXIST 1
#define ERR_EMPTY_GROUP 2
#define ERR_EMPTY_STMT 3
#define ERR_ARRAY_EMPTY 4
#define ERR_ARRAY_CLOSING 5
#define ERR_NAKED_VARIABLE 6
#define ERR_INVALID_TYPES 7

// These are the errors a parser may generate. They are mapped to the #DEFINE above.
static const char *Error_Templates[] = {
	"* Parsing error : unexpected @0 found in line @1",
	"* Notice : Duplicate definition {@0} already defined in line @1",
	"* Notice : Empty group {@0} must contain commands in line @1",
	"* Parsing error: Statement '@0' missing arguments in line @1",
	"* Parsing error : Expected array item after comma but found NULL near @0 in line @1",
	"* Syntax error: No closing bracket found near '@0' in line @1",
	"* Parsing error: '$@0' declaraion must be followed by valid assignment in line @1",
	"* Cannot perform operation on incompatible types near @0",
};

// Sync ParserMgr internal token to be current token held by TokenMgr.
static void par_mgr_sync(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_current_token(par_mgr->tok_mgr);
}

// Increment the token within TokenMgr and assign it to par_mgr.
static void par_mgr_next(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
}

// Check to make sure operation is one of (== != <= >= < >)
static int is_compare_operator(char *op) {
	return (string_compare(op, "!=") 
			|| string_compare(op, "==") 
			|| string_compare(op, "<") 
			|| string_compare(op, "<=")
			|| string_compare(op, ">")  
			|| string_compare(op, ">=")
			|| string_compare(op, "><"));
}

// Return the type of compare node based on token.
static enum NodeType get_compare_type(char *op) {
	if (string_compare(op, "==")) {
		return E_EEQUAL_NODE;
	}
	else if (string_compare(op, "!=")) {
		return E_NEQUAL_NODE;
	}
	else if (string_compare(op, "<")) {
		return E_LESSTHAN_NODE;
	}
	else if (string_compare(op, "<=")) {
		return E_LESSTHANEQ_NODE;
	}
	else if (string_compare(op, ">")) {
		return E_GREATERTHAN_NODE;
	}
	else if (string_compare(op, ">=")) {
		return E_GREATERTHANEQ_NODE;
	}
	else if (string_compare(op, "><")) {
		return E_BETWEEN_NODE;
	}

	return E_EOF_NODE;
}

// Allocate more memory for array node items.
static Node **grow_arr_nodes(Node *arr_node) {
	if (null_check(arr_node, "grow array nodes")) return NULL;
	Node **new_items = NULL;
	arr_node->data->ArrayNode.dcap = arr_node->data->ArrayNode.dcap * (arr_node->data->ArrayNode.dcap / 2);
	new_items = realloc(arr_node->data->ArrayNode.items, arr_node->data->ArrayNode.dcap * sizeof(Node *));
	return new_items;
}

// Shorthand for malloc'ed array node and items.
static Node *node_new_array() {
	Node *arr = NULL;
	arr = Node_new(1);
	arr->type = E_ARRAY_NODE;
	arr->data->ArrayNode.dctr = 0;
	arr->data->ArrayNode.dcap = 15;
	arr->value = NULL;
	arr->data->ArrayNode.items = malloc(arr->data->ArrayNode.dcap * sizeof(Node *));
	return arr;
}

ParserMgr *ParserMgr_new() {
	ParserMgr *ps = malloc(sizeof(ParserMgr));
	ps->curr_token = NULL;
	ps->node_mgr = NULL;
	ps->tok_mgr = NULL;
	ps->err_handle = NULL;
	return ps;
}

int ParserMgr_free(ParserMgr *par_mgr) {
	if (null_check(par_mgr, "parsermgr free")) return -1;
		
	par_mgr->curr_token = NULL;
	par_mgr->err_handle = NULL;
	par_mgr->tok_mgr = NULL;
	free(par_mgr);

	return 0;
}

void ParserMgr_add_error(Error *err_handle, Token *offender, int err_type) {
	// Don't exceed error limit.
	if (err_handle->error_cap == err_handle->error_ctr)
		return;
	
	// String representation of offending line number. 
	char off_lineno[16]; // Assume that a source file won't exceed 9999,9999,9999,9999 lines ?
	// Pointer to offending value.
	char *off_value = offender->value;
	// The error template which is needed.
	const char *template = Error_Templates[err_type];
	// Array containing string of substitute values.
	char *template_values[] = {off_value, off_lineno};
	// Final template error.
	char *template_fmt = NULL;

	// We can't convert line number to string so replace with undefined.	
	if (int_to_string(off_lineno, offender->lineno) < 0)
		strncpy(off_lineno, "undefined", 10);

	template_fmt = string_map_vars(template, template_values, strlen(template), 2);
	
	// Add the final template string to error handler.
	err_handle->errors[err_handle->error_ctr] = template_fmt;
	err_handle->error_ctr++;
}

void ParserMgr_skip_to(ParserMgr *par_mgr, char *type) {
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && !string_compare(type, par_mgr->curr_token->value)) {
		par_mgr_next(par_mgr);
	}
}

Node *parse_string(ParserMgr *par_mgr) {
	Node *str = NULL;
	if (string_compare(par_mgr->curr_token->type, "STRING") || string_compare(par_mgr->curr_token->type, "MIXSTRING")) {
		str = Node_new(0);
		str->type = E_STRING_NODE;
		
		// Change type if mix string.
		if (string_compare(par_mgr->curr_token->type, "MIXSTRING"))
			str->type = E_MIXSTR_NODE;
			
		str->value = par_mgr->curr_token->value;
		par_mgr_next(par_mgr);
	}
	return str;
}

Node *parse_factor(ParserMgr *par_mgr) {
	par_mgr_sync(par_mgr);
	Node *res = NULL;
	 if (string_compare(par_mgr->curr_token->type, "INTEGER")) {
		 res = Node_new(0);
		 res->type = E_INTEGER_NODE;
		 res->value = par_mgr->curr_token->value; 
		 par_mgr_next(par_mgr);
	 }
	 else if (string_compare(par_mgr->curr_token->type, "IDENTIFIER")) {
		 res = Node_new(0);
		 res->type = E_IDENTIFIER_NODE;
		 res->value = par_mgr->curr_token->value; 
		 par_mgr_next(par_mgr);
	 }
	 else if (string_compare(par_mgr->curr_token->type, "LPAREN")) {
		 TokenMgr_next_token(par_mgr->tok_mgr);
		 res = parse_expr(par_mgr);
		 par_mgr_sync(par_mgr);
		 if (string_compare(par_mgr->curr_token->type, "RPAREN")) {
			par_mgr_next(par_mgr); 
		 }
	 }
	 return res;
}

Node *parse_term(ParserMgr *par_mgr) {
	Node *res = parse_factor(par_mgr);
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) 
		&& (string_compare(par_mgr->curr_token->value, "*")
		|| string_compare(par_mgr->curr_token->value, "/"))){
		
		// Operation node.
		Node *bop = Node_new(1);

		if (strncmp(par_mgr->curr_token->value, "*", 1) == 0) {
			bop->type = E_TIMES_NODE;
			bop->value = "*"; 
		}
		else if (strncmp(par_mgr->curr_token->value, "/", 1) == 0){
			bop->type = E_DIV_NODE;
			bop->value = "/"; 
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
			continue;
		}
		TokenMgr_next_token(par_mgr->tok_mgr);
		bop->data->BinExpNode.left = res;
		bop->data->BinExpNode.right = parse_factor(par_mgr);

		// Ensure right operand.
		if (!bop->data->BinExpNode.right) {
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
			continue;
		}

		par_mgr->expr_depth++;
		res = bop;
	}
	
	return res;
}

Node *parse_expr(ParserMgr *par_mgr) {
	Node *res = parse_term(par_mgr);
	
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) 
		&&	(string_compare(par_mgr->curr_token->value, "+") 
		|| string_compare(par_mgr->curr_token->value, "-")
		|| is_compare_operator(par_mgr->curr_token->value))) {
		
		// Operation node.
		Node *bop = Node_new(1);

		if (strncmp(par_mgr->curr_token->value, "-", 1) == 0) {
			bop->type = E_MINUS_NODE;
			bop->value = "-"; 
		}
		else if (strncmp(par_mgr->curr_token->value, "+", 1) == 0){
			bop->type = E_ADD_NODE;
			bop->value = "+"; 
		}
		else if (is_compare_operator(par_mgr->curr_token->value)) {	
			bop->value = par_mgr->curr_token->value;
			bop->type = get_compare_type(bop->value);
		} 
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
			continue;
		}
		
		TokenMgr_next_token(par_mgr->tok_mgr);
		bop->data->BinExpNode.left = res;

		// Set right node based on type.
		if (is_compare_operator(par_mgr->curr_token->value))
			bop->data->BinExpNode.right = parse_expr(par_mgr);
		else 
			bop->data->BinExpNode.right = parse_term(par_mgr);

		// Ensure right operand.
		if (!bop->data->BinExpNode.right) {
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_UNEXPECTED);
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_INVALID_TYPES);
			TokenMgr_next_token(par_mgr->tok_mgr);
			continue;
		}

		par_mgr->expr_depth++;
		res = bop;
	}
	
	return res;
}

Node *parse_array(ParserMgr *par_mgr) {
	
	// Store retrun node from each root level evaluation.
	Node *ret = NULL;
	// Store final array node.
	Node *arr = NULL;

	// Array must begin with '['
	if (!string_compare(par_mgr->curr_token->type, "LBRACKET"))
		return NULL;

	// Push token pointer forward.
	par_mgr_next(par_mgr);	

	// Handle first element.
	if (string_compare(par_mgr->curr_token->type, "STRING")) {
		ret = parse_string(par_mgr);
	}
	else if (string_compare(par_mgr->curr_token->type, "INTEGER")) {
		ret = parse_factor(par_mgr);
	}
	if (string_compare(par_mgr->curr_token->type, "LBRACKET")) {
		ret = parse_array(par_mgr);
	}
	
	// Instansiate array node.
	// Add node to array node if valid.
	arr = node_new_array();
	if (ret)
		arr->data->ArrayNode.items[arr->data->ArrayNode.dctr++] = ret;
	
	// Iterate using comma as delimiter.
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) 
		&& (string_compare(par_mgr->curr_token->type, "COMMA"))) {
		
		// Next token.
		par_mgr_next(par_mgr);		

		if (string_compare(par_mgr->curr_token->type, "INTEGER")) {
			ret = parse_factor(par_mgr);
		}
		else if (string_compare(par_mgr->curr_token->type, "STRING")) {
			ret = parse_string(par_mgr);
		}
		else if (string_compare(par_mgr->curr_token->type, "LBRACKET")) {
			ret = parse_array(par_mgr);
		}
		else {
			ret = NULL;
		}

		// Ignore empty values after comma.
		if (!ret) {
			continue;
		}

		// Resize if need be, prior to appending array node.
		if (arr->data->ArrayNode.dcap - arr->data->ArrayNode.dctr <= 5)
			arr->data->ArrayNode.items = grow_arr_nodes(arr);

		arr->data->ArrayNode.items[arr->data->ArrayNode.dctr++] = ret;	
	}
	
	if (!string_compare(par_mgr->curr_token->type, "RBRACKET"))
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_ARRAY_CLOSING);
	else
		par_mgr_next(par_mgr);
	
	return arr;

}

Node *parse_assignment(ParserMgr *par_mgr) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(par_mgr->tok_mgr);
	// Store pointer to subsequent tokens.
	par_mgr_next(par_mgr);
	// Reset expression depth.
	par_mgr->expr_depth = 0;
	
	// Final ast build by entire assignment.
	Node *ast = NULL;
	// Returned by each recursive handle.
	Node *expr = NULL;	
	// Leftmost node of root ast. 
	Node *lhand = NULL;

	// Assert we can consume an EQUAL.
	if (par_mgr->curr_token && string_compare(par_mgr->curr_token->value, "=")) {
		par_mgr_next(par_mgr);
		if ((expr = parse_string(par_mgr)) || (expr = parse_expr(par_mgr)) || (expr = parse_array(par_mgr))) {

			// Add symbol if not exits.
			if (!SyTable_get_symbol(par_mgr->sy_table, tok_start_ptr->value))
				SyTable_add_symbol(par_mgr->sy_table, tok_start_ptr, E_IDN_TYPE);
			
			// Identifier.
			lhand = Node_new(0); 
			lhand->type = E_IDENTIFIER_NODE;
			lhand->value = tok_start_ptr->value;

			// Join to return ast from expression.
			ast = Node_new(1); 
			ast->type = E_EQUAL_NODE;
			ast->value = "=";
			ast->data->AsnStmtNode.left = lhand;
			ast->data->AsnStmtNode.right = expr;
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_UNEXPECTED);
			par_mgr_next(par_mgr);
		}
	}
	else {
		ParserMgr_add_error(par_mgr->err_handle,TokenMgr_prev_token(par_mgr->tok_mgr), ERR_NAKED_VARIABLE);
		par_mgr_next(par_mgr);
	}
	return ast;
}

Node *parse_group(ParserMgr *par_mgr) {
	// If string isn't next then store error and move to next token.
	if (!string_compare(TokenMgr_peek_token(par_mgr->tok_mgr)->type, "STRING")) {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_EMPTY_GROUP);
		TokenMgr_next_token(par_mgr->tok_mgr);
		return NULL;
	}

	// Check if group already defined. Add it to sytable if it doesn't.
	// Otherwise store "duplicate group" error.
	if (SyTable_get_symbol(par_mgr->sy_table, par_mgr->curr_token->value)) {
		ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_GROUP_EXIST);
		par_mgr_next(par_mgr);
		return NULL;
	}

	// Group node itself. i.e {some_group}.
	Node *group = Node_new(1);
	// Previously read command <string>.
	Node *prev = NULL;
	// Recently read command <string>
	Node *curr = NULL;
	
	// Create group entry.
	SyTable_add_symbol(par_mgr->sy_table, par_mgr->curr_token, E_GROUP_TYPE);
	
	par_mgr_sync(par_mgr);
	
	// Setup group node data.
	group->value = par_mgr->curr_token->value;
	group->type = E_GROUP_NODE;

	// Store next token in parser state.
	par_mgr->curr_token =  TokenMgr_next_token(par_mgr->tok_mgr);

	// Iterate through commands and append to group.
	// Below will build a circular single linked list.
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && string_compare(par_mgr->curr_token->type, "STRING")) {
		curr = parse_string(par_mgr);
		curr->data = malloc(sizeof(union SyntaxNode));
		if (!prev)
			group->data->GroupNode.next = curr;
		else
			prev->data->GroupNode.next = curr;
		prev = curr;
	}

	// Point final command back to group node.
	curr->data->GroupNode.next = group;
	prev = NULL;
	curr = NULL;
	return group;
}

Node *parse_keyword(ParserMgr *par_mgr) {
	// Peek into the next token in TokenMgr.
	Token *peek = TokenMgr_peek_token(par_mgr->tok_mgr);

	// If valid arg isn't next then store error and move to next token.
	if (!string_compare(peek->type, "STRING")
	&& !string_compare(peek->type, "MIXSTRING")
		&& !string_compare(peek->type, "INTEGER")
		&& !string_compare(peek->type, "IDENTIFIER")) {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_EMPTY_STMT);
		par_mgr_next(par_mgr);
		return NULL;
	}

	// Store keyword argument.
	Node *args = NULL;
	// Final statement node,
	Node *stmt = NULL;
	// Store function token.
	Token *name = par_mgr->curr_token;

	par_mgr_next(par_mgr);

	// If args is valid then store.
	if ((args = parse_expr(par_mgr)) || (args = parse_string(par_mgr))) {
		stmt = Node_new(1);
		stmt->type = E_FUNC_NODE;
		stmt->value = name->value;
		stmt->data->FuncNode.args = args;
	}
	else {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_UNEXPECTED);
		par_mgr_next(par_mgr);
	}

	return stmt;
}

ParserMgr *ParseMgr_init(TokenMgr *tok_mgr, SyTable *sy_table, NodeMgr *node_mgr, Error *err) {
	if (!tok_mgr || !sy_table || !node_mgr || !err) return NULL;

	ParserMgr *par_mgr = ParserMgr_new();
	par_mgr->tok_mgr = tok_mgr;
	par_mgr->sy_table = sy_table;
	par_mgr->err_handle = err;
	par_mgr->node_mgr = node_mgr;
	par_mgr_sync(par_mgr);
	return  par_mgr;
}

Node *Parser_parse(ParserMgr *par_mgr) {
	if (null_check(par_mgr, "parser parse")) return NULL;

	// Store resulting tree.
	Node *ast = NULL;

	while (!TokenMgr_is_last_token(par_mgr->tok_mgr)) {
		if (strcmp(par_mgr->curr_token->type, "IDENTIFIER") == 0) {
			ast = parse_assignment(par_mgr);
		}
		else if (strcmp(par_mgr->curr_token->type, "GROUP") == 0) {
			ast = parse_group(par_mgr);
		}
		else if (strcmp(par_mgr->curr_token->type, "KEYWORD") == 0) {
			ast = parse_keyword(par_mgr);
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			par_mgr_next(par_mgr);
			continue;
		}

		if (ast != NULL) {
			ast->depth = par_mgr->expr_depth;
			NodeMgr_add_node(par_mgr->node_mgr, ast);
		}

		par_mgr_sync(par_mgr);
	}
	
	Error_print_all(par_mgr->err_handle);

	return ast;
}
