#include <stdio.h>
#include <stdlib.h>
#include "utils.h"	
#include "parser.h"
#include "tokens.h"
#include "tokenizer.h"   
#include "node.h"

// Below are the errors which map to Error_Templates.
#define ERR_UNEXPECTED 0
#define ERR_GROUP_EXIST 1
#define ERR_EMPTY_GROUP 2

// Initial amount of errors stored.
#define INIT_MAX_ERRORS 20

// These are the errors a parser may generate. They are mapped to the #DEFINE above.
static const char *Error_Templates[] = {
	"unexpected @0 found in line @1",
	"duplicate definition {@0} already defined in line @1",
	"empty group {@0} must contain commands in line @1",
};

PErrors *parser_new_perrors(void) {
	PErrors *err_handle = malloc(sizeof(PErrors));
	err_handle->error_cap = INIT_MAX_ERRORS;
	err_handle->error_ctr = 0;
	err_handle->errors = malloc(sizeof(char *) * err_handle->error_cap);
	return err_handle;
}

void parser_free_perrors(PErrors *err_handle, int print_mode) {
	if (err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {
			if (print_mode)
				printf("%s\n",err_handle->errors[in]);
			free(err_handle->errors[in]);
		}
		free(err_handle);
	}
}

void parser_add_perror(PErrors *err_handle, Token *offender, int err_type) {
	// Determine if we need to make bigger.
	if (err_handle->error_cap - err_handle->error_ctr <= 5) {
		err_handle->error_cap  = err_handle->error_cap + err_handle->error_ctr / 2;
		char **errors_n = realloc(err_handle->errors, err_handle->error_cap * sizeof(char *));
		err_handle->errors = errors_n;
	}
	
	// String representation of offending line number. 
	char off_lineno[16]; // TODO: Assume that a source file won't exceed 9999,9999,9999,9999 lines ?
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
	err_handle->errors[err_handle->error_ctr] = malloc(strlen(template_fmt) * sizeof(char));
	strcpy(err_handle->errors[err_handle->error_ctr], template_fmt);
	err_handle->error_ctr++;
}

int parser_can_consume(char *tok_type, char *type) {
	if (strcmp(tok_type, type) != 0)
		return 0;
	
	return 1;
}

struct Node *parse_string(TokenMgr *tok_mgr) {
	struct Node *str = NULL;
	Token *tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	if (parser_can_consume(tok_curr_ptr->type, "STRING")) {
		str = Node_new(0);
		str->type = E_STRING_NODE;
		str->value = tok_curr_ptr->value;
		// Iterate to next token.
		TokenMgr_next_token(tok_mgr);
	}
	return str;
}

struct Node *parse_factor(TokenMgr *tok_mgr) {
	Token *tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	struct Node *res = NULL;
	 if (parser_can_consume(tok_curr_ptr->type, "INTEGER")) {
		 res = Node_new(0);
		 res->type = E_INTERGER_NODE;
		 res->value = tok_curr_ptr->value; 
		 TokenMgr_next_token(tok_mgr);
	 }
	//  else if (can_consume(tok_curr_ptr->type, "LPAREN") == 0) {
	// 	 TokenMgr_next_token(tok_mgr);
	// 	 res = parse_expr(tok_mgr);
	// 	 tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	// 	 if (can_consume(tok_curr_ptr->type, "RPAREN") == 0) {
	// 		TokenMgr_next_token(tok_mgr); 
	// 	 }
	//  }
	 return res;
}

// struct Node *parse_term(TokenMgr *tok_mgr) {
// 	struct Node *res = parse_factor(tok_mgr);
// 	Token *tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	
// 	// Handle expression calculation.
// 	while (strncmp(tok_curr_ptr->value, "ASTERISK", tok_curr_ptr->val_length) == 0 || 
// 		strncmp(tok_curr_ptr->value, "FSLASH", tok_curr_ptr->val_length) == 0) {
// 		if (strncmp(tok_curr_ptr->value, "ASTERISK", tok_curr_ptr->val_length) == 0 && 
// 			can_consume(tok_curr_ptr->value, "ASTERISK") == 0) {
// 			tok_curr_ptr = TokenMgr_next_token(tok_mgr);
// 			res = res * parse_factor(tok_mgr);
// 		}
// 		else if (strncmp(tok_curr_ptr->value, "FSLASH", tok_curr_ptr->val_length) == 0 && 
// 			can_consume(tok_curr_ptr->value, "FSLASH") == 0) {
// 			tok_curr_ptr = TokenMgr_next_token(tok_mgr);
// 			res = res / parse_factor(tok_mgr);
// 		}
// 		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
// 	}
// 	return res;
// }

struct Node *parse_expr(TokenMgr *tok_mgr) {
	// Build integer node with first number.
	struct Node *res = parse_factor(tok_mgr);
	// Pointer to the most recent expression ast.
	struct Node *curr_expr = NULL;
	// Pointer to the root/first expression ast.
	struct Node *root_expr = NULL;
	// Pointer to track intermediary expressions.
	struct Node *expr_itr = NULL;
	// How deep the right hand nodes are.
	unsigned int expr_depth = 0;

	// Track current token in tok_mgr.
	Token *tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	
	while (!TokenMgr_is_last_token(tok_mgr) && 
			(parser_can_consume(tok_curr_ptr->value, "-") || parser_can_consume(tok_curr_ptr->value, "+"))
			&& res != NULL) {

		// Create expression node foreach iteration.
		curr_expr = Node_new(1);

		// Set node type.
		if (strncmp(tok_curr_ptr->value, "+", 1) == 0) {
			curr_expr->type = E_ADD_NODE;
		}
		else if (strncmp(tok_curr_ptr->value, "-", 1) == 0) {
			curr_expr->type = E_MINUS_NODE;
		}
		
		// Set left value of expression node. 
		// 1 + 5 -> left value = 1.
		curr_expr->data->BinExpNode.left = res;

		// Parse next token.
		// Should be number.
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
		res = parse_factor(tok_mgr);

		// Set right value of expression node.
		// 1 + 5 -> right value = 5
		curr_expr->data->BinExpNode.right = res;
		
		// Is it a tree more than 1 nesting of arithmetics.
		if (root_expr != NULL)
			expr_itr->data->BinExpNode.right = curr_expr;
		else
			root_expr = curr_expr;
		
		expr_depth++;
		expr_itr = curr_expr;
		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	}

	expr_itr = NULL;
	curr_expr = NULL;

	if (root_expr != NULL) {
		root_expr->depth = expr_depth;
		res = root_expr;
		root_expr = NULL;
	}
	
	return res;
		
}

struct Node *parse_assignment(TokenMgr *tok_mgr, PErrors *err_handle) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(tok_mgr);
	// Store pointer to subsequent tokens.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	// Final ast build by entire assignment.
	struct Node *ast = NULL;
	// Returned by each recursive handle.
	struct Node *expr = NULL;	
	// Leftmost node of root ast. 
	struct Node *lhand = NULL;

	// Assert we can consume an EQUAL.
	if (parser_can_consume(tok_curr_ptr->value, "=")) {
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
		if ((expr = parse_expr(tok_mgr)) != NULL || (expr = parse_string(tok_mgr)) != NULL) {
			// Identifier.
			lhand = Node_new(0); 
			lhand->type = E_IDENTIFIER_NODE;
			lhand->value = tok_start_ptr->value;

			// Join to return ast from expression.
			ast = Node_new(1); 
			ast->type = E_EQUAL_NODE;
			ast->data->AsnStmtNode.left = lhand;
			ast->data->AsnStmtNode.right = expr;
			ast->depth = expr->depth + 1;
		}
		else {
			parser_add_perror(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
			TokenMgr_next_token(tok_mgr);
		}
	}
	else {
		parser_add_perror(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
		TokenMgr_next_token(tok_mgr);
	}

	return ast;
}

struct Node *parse_group(TokenMgr *tok_mgr, PErrors *err_handle) {
	// If string isn't next then store error and move to next token.
	if (strcmp(TokenMgr_peek_token(tok_mgr)->type, "STRING") != 0) {
		parser_add_perror(err_handle, TokenMgr_current_token(tok_mgr), ERR_EMPTY_GROUP);
		TokenMgr_next_token(tok_mgr);
		return NULL;
	}

	struct Node *group_root = Node_new(1);
	struct Node *prev = NULL;
	struct Node *curr = NULL;
	
	// Setup root node data.
	group_root->value = TokenMgr_current_token(tok_mgr)->value;
	group_root->type = E_GROUP_NODE;
	Token *tok_curr_ptr =  TokenMgr_next_token(tok_mgr);

	// Initialise nodes.
	while (!TokenMgr_is_last_token(tok_mgr) && parser_can_consume(tok_curr_ptr->type, "STRING")) {
		curr = parse_string(tok_mgr);
		curr->data = malloc(sizeof(union SyntaxNode));
		if (prev == NULL)
			group_root->data->GroupNode.next = curr;
		else
			prev->data->GroupNode.next = curr;
		prev = curr;
		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	}

	// Point final command to root.
	// Creates circular list.
	curr->data->GroupNode.next = group_root;

	return group_root;
}

int parser_init(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		return -1;
	}

	PErrors *err_handle = parser_new_perrors();

	if (err_handle == NULL)
		return -1;

	NodeMgr *node_mgr = NodeMgr_new();
	struct Node *ret_node = NULL; 

	// Shorthand pointer to current token.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	while (!TokenMgr_is_last_token(tok_mgr)) {
		if (strcmp(tok_curr_ptr->type, "IDENTIFIER") == 0) {
			ret_node = parse_assignment(tok_mgr, err_handle);
		}
		else if (strcmp(tok_curr_ptr->type, "GROUP") == 0) {
			ret_node = parse_group(tok_mgr, err_handle);
		}
		else {
			parser_add_perror(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
			TokenMgr_next_token(tok_mgr);
		}
		
		if (ret_node != NULL)
			NodeMgr_add_node(node_mgr, ret_node);
	
		// Set current pointer to tok_mgr current token.
		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	}

	parser_free_perrors(err_handle, 1);
	NodeMgr_free(node_mgr);
	
	if (err_handle->error_ctr > 0)
		return 1;

	return 0;
}