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

ParserState *ParserState_new() {
	ParserState *ps = malloc(sizeof(ParserState));
	ps->curr_expr = NULL;
	ps->expr_itr = NULL;
	ps->curr_token = NULL;
	ps->tok_mgr = NULL;
	ps->err_handle = NULL;
	ps->root_expr = NULL;
	ps->expr_itr = 0;
	ps->lstate = 0;
	ps->expr_depth = 0;
	return ps;
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

struct Node *parse_string(ParserState *pstate) {
	struct Node *str = NULL;
	if (parser_can_consume(pstate->curr_token->type, "STRING")) {
		str = Node_new(0);
		str->type = E_STRING_NODE;
		str->value = pstate->curr_token->value;
		pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
	}
	return str;
}

struct Node *parse_factor(ParserState *pstate) {
	pstate->curr_token = TokenMgr_current_token(pstate->tok_mgr);
	struct Node *res = NULL;
	 if (parser_can_consume(pstate->curr_token->type, "INTEGER")) {
		 res = Node_new(0);
		 res->type = E_INTERGER_NODE;
		 res->value = pstate->curr_token->value; 
		 pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
	 }
	//  else if (parser_can_consume(pstate->curr_token->type, "LPAREN")) {
	// 	 TokenMgr_next_token(pstate->tok_mgr);
	// 	 res = parse_expr(pstate);
	// 	 pstate->curr_token = TokenMgr_current_token(pstate->tok_mgr);
	// 	 if (parser_can_consume(pstate->curr_token->type, "RPAREN")) {
	// 		pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr); 
	// 	 }
	//  }
	 return res;
}

struct Node *parse_term(ParserState *pstate) {
	struct Node *res = parse_factor(pstate);

	if (pstate->curr_expr != NULL) {
		pstate->root_expr = pstate->curr_expr;
	}

	while (!TokenMgr_is_last_token(pstate->tok_mgr) && 
			(parser_can_consume(pstate->curr_token->value, "*") || parser_can_consume(pstate->curr_token->value, "/"))
			&& res != NULL) {

		// Create expression node foreach iteration.
		res = Node_new(1);

		// Set node type.
		if (strncmp(pstate->curr_token->value, "*", 1) == 0) {
			res->type = E_TIMES_NODE;
		}
		else if (strncmp(pstate->curr_token->value, "/", 1) == 0) {
			res->type = E_DIV_NODE;
		}
	}

	return res;
}

struct Node *parse_expr(ParserState *pstate) {
	struct Node *res = parse_term(pstate);

	while (!TokenMgr_is_last_token(pstate->tok_mgr) && 
			(parser_can_consume(pstate->curr_token->value, "-") || parser_can_consume(pstate->curr_token->value, "+"))
			&& res != NULL) {

		// Create expression node foreach iteration.
		pstate->curr_expr = Node_new(1);

		// Set node type.
		if (strncmp(pstate->curr_token->value, "+", 1) == 0) {
			pstate->curr_expr->type = E_ADD_NODE;
		}
		else if (strncmp(pstate->curr_token->value, "-", 1) == 0) {
			pstate->curr_expr->type = E_MINUS_NODE;
		}
		
		if (res->type == E_INTERGER_NODE && !pstate->lstate) {
				pstate->curr_expr->data->BinExpNode.left = res;
				pstate->lstate = 1;
		} 
		else {
			pstate->curr_expr->data->BinExpNode.right = res;
			pstate->lstate = 0;
			pstate->expr_depth++;
		}
			
		pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
		res = parse_term(pstate);
		
		if (res->type == E_INTERGER_NODE && !pstate->lstate) {
			pstate->curr_expr->data->BinExpNode.left = res;
			pstate->lstate = 1;
		} 
		else {
			pstate->curr_expr->data->BinExpNode.right = res;
			pstate->lstate = 0;
			pstate->expr_depth++;
		}
					
		// Is it a tree more than 1 nesting of arithmetics.
		if (pstate->expr_depth > 1 ) {
			pstate->expr_itr->data->BinExpNode.right = pstate->curr_expr;
		}		
		else {
			pstate->root_expr = pstate->curr_expr;
		}

		pstate->expr_itr = pstate->curr_expr;
			
	}

	// Set root expression node.
	// pstate->root_expr = pstate->expr_itr;
	// pstate->root_expr->depth = pstate->expr_depth;

	// Clean up unused pointers.
	pstate->expr_itr = NULL;
	pstate->curr_expr = NULL;
	res = NULL;
	
	return pstate->root_expr;
}

struct Node *parse_assignment(ParserState *pstate) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(pstate->tok_mgr);
	// Store pointer to subsequent tokens.
	pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
	
	// Final ast build by entire assignment.
	struct Node *ast = NULL;
	// Returned by each recursive handle.
	struct Node *expr = NULL;	
	// Leftmost node of root ast. 
	struct Node *lhand = NULL;

	// Assert we can consume an EQUAL.
	if (parser_can_consume(pstate->curr_token->value, "=")) {
		pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
		if ((expr = parse_expr(pstate)) != NULL || (expr = parse_string(pstate)) != NULL) {
			// Identifier.
			lhand = Node_new(0); 
			lhand->type = E_IDENTIFIER_NODE;
			lhand->value = tok_start_ptr->value;

			// Join to return ast from expression.
			ast = Node_new(1); 
			ast->type = E_EQUAL_NODE;
			ast->value = "+";
			ast->data->AsnStmtNode.left = lhand;
			ast->data->AsnStmtNode.right = expr;
			ast->depth = expr->depth + 1;
		}
		else {
			parser_add_perror(pstate->err_handle, pstate->curr_token, ERR_UNEXPECTED);
			pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
		}
	}
	else {
		parser_add_perror(pstate->err_handle, pstate->curr_token, ERR_UNEXPECTED);
		pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
	}
	return ast;
}

struct Node *parse_group(ParserState *pstate) {
	// If string isn't next then store error and move to next token.
	if (strcmp(TokenMgr_peek_token(pstate->tok_mgr)->type, "STRING") != 0) {
		parser_add_perror(pstate->err_handle, TokenMgr_current_token(pstate->tok_mgr), ERR_EMPTY_GROUP);
		TokenMgr_next_token(pstate->tok_mgr);
		return NULL;
	}

	struct Node *group_root = Node_new(1);
	struct Node *prev = NULL;
	struct Node *curr = NULL;
	
	// Setup root node data.
	group_root->value = TokenMgr_current_token(pstate->tok_mgr)->value;
	group_root->type = E_GROUP_NODE;

	// Store next token in parser state.
	pstate->curr_token =  TokenMgr_next_token(pstate->tok_mgr);

	// Initialise nodes.
	while (!TokenMgr_is_last_token(pstate->tok_mgr) && parser_can_consume(pstate->curr_token->type, "STRING")) {
		curr = parse_string(pstate);
		curr->data = malloc(sizeof(union SyntaxNode));
		if (prev == NULL)
			group_root->data->GroupNode.next = curr;
		else
			prev->data->GroupNode.next = curr;
		prev = curr;
	}

	// Point final command to root.
	// Creates circular list.
	curr->data->GroupNode.next = group_root;
	return group_root;
}

int parser_init(TokenMgr *tok_mgr) {
	// Create wrapper structs.
	ParserState *pstate = ParserState_new();
	pstate->err_handle = parser_new_perrors();
	NodeMgr *node_mgr = NodeMgr_new();

	// This will do for now.
	// TODO: Add descriptive error here.
	if (pstate->err_handle == NULL || pstate == NULL || node_mgr == NULL || tok_mgr == NULL)
		return -1;
		
	pstate->tok_mgr = tok_mgr;
	tok_mgr = NULL;

	// Shorthand pointer to current token.
	pstate->curr_token = TokenMgr_next_token(pstate->tok_mgr);
	
	while (!TokenMgr_is_last_token(pstate->tok_mgr)) {
		if (strcmp(pstate->curr_token->type, "IDENTIFIER") == 0) {
			pstate->curr_expr = parse_assignment(pstate);
		}
		else if (strcmp(pstate->curr_token->type, "GROUP") == 0) {
			pstate->curr_expr = parse_group(pstate);
		}
		else {
			parser_add_perror(pstate->err_handle, pstate->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(pstate->tok_mgr);
		}
		
		if (pstate->curr_expr != NULL)
			NodeMgr_add_node(node_mgr, pstate->curr_expr);
	}

	int err = pstate->err_handle->error_ctr;

	// Free resources.
	parser_free_perrors(pstate->err_handle, 1);
	NodeMgr_free(node_mgr);
	free(pstate);

	if (err > 0)
		return 1;

	return 0;
}