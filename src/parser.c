#include <stdio.h>
#include <stdlib.h>
#include "utils.h"	
#include "parser.h"
#include "tokenizer.h"   
#include "node.h"
#include "errors.h"

// Below are the errors which map to Error_Templates.
#define ERR_UNEXPECTED 0
#define ERR_GROUP_EXIST 1
#define ERR_EMPTY_GROUP 2

// These are the errors a parser may generate. They are mapped to the #DEFINE above.
static const char *Error_Templates[] = {
	"unexpected @0 found in line @1",
	"duplicate definition {@0} already defined in line @1",
	"empty group {@0} must contain commands in line @1",
};

ParserMgr *ParserMgr_new() {
	ParserMgr *ps = malloc(sizeof(ParserMgr));
	ps->curr_expr = NULL;
	ps->curr_token = NULL;
	ps->tok_mgr = NULL;
	ps->err_handle = NULL;
	return ps;
}

int *ParserMgr_free(ParserMgr *par_mgr) {
	if (par_mgr == NULL)
		return 1;
		
	par_mgr->curr_expr = NULL;
	par_mgr->curr_token = NULL;
	par_mgr->err_handle = NULL;
	par_mgr->tok_mgr = NULL;
	free(par_mgr);
}

void ParserMgr_add_error(Error *err_handle, Token *offender, int err_type) {
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

struct Node *parse_string(ParserMgr *par_mgr) {
	struct Node *str = NULL;
	if (parser_can_consume(par_mgr->curr_token->type, "STRING")) {
		str = Node_new(0);
		str->type = E_STRING_NODE;
		str->value = par_mgr->curr_token->value;
		par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
	}
	return str;
}

struct Node *parse_factor(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_current_token(par_mgr->tok_mgr);
	struct Node *res = NULL;
	 if (parser_can_consume(par_mgr->curr_token->type, "INTEGER")) {
		 res = Node_new(0);
		 res->type = E_INTERGER_NODE;
		 res->value = par_mgr->curr_token->value; 
		 par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
	 }
	//  else if (parser_can_consume(par_mgr->curr_token->type, "LPAREN")) {
	// 	 TokenMgr_next_token(par_mgr->tok_mgr);
	// 	 res = parse_expr(par_mgr);
	// 	 par_mgr->curr_token = TokenMgr_current_token(par_mgr->tok_mgr);
	// 	 if (parser_can_consume(par_mgr->curr_token->type, "RPAREN")) {
	// 		par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr); 
	// 	 }
	//  }
	 return res;
}

// struct Node *parse_term(ParserMgr *par_mgr) {
// 	struct Node *res = parse_factor(par_mgr);


// 	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && 
// 			(parser_can_consume(par_mgr->curr_token->value, "*") || parser_can_consume(par_mgr->curr_token->value, "/"))
// 			&& res != NULL) {

// 		// Create expression node foreach iteration.
// 		res = Node_new(1);

// 		// Set node type.
// 		if (strncmp(par_mgr->curr_token->value, "*", 1) == 0) {
// 			res->type = E_TIMES_NODE;
// 		}
// 		else if (strncmp(par_mgr->curr_token->value, "/", 1) == 0) {
// 			res->type = E_DIV_NODE;
// 		}
// 		else {
// 			parser_add_perror(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
// 			par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
// 			continue;
// 		}

// 		if (res->type == E_INTERGER_NODE && !par_mgr->lnode_state) {
// 				par_mgr->curr_expr->data->BinExpNode.left = res;
// 				par_mgr->lnode_state = 1;
// 		} 
// 		else {
// 			par_mgr->curr_expr->data->BinExpNode.right = res;
// 			par_mgr->lnode_state = 0;
// 			par_mgr->expr_depth++;
// 		}

// 		res = parse_factor(par_mgr);

// 	}

// 	return res;
// }

struct Node *parse_expr(ParserMgr *par_mgr) {
	struct Node *res = parse_factor(par_mgr);
	struct Node *binop = NULL;
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && 
			(parser_can_consume(par_mgr->curr_token->value, "-") || parser_can_consume(par_mgr->curr_token->value, "+"))
			&& res != NULL) {

		// Create binop node foreach iteration.
		binop = Node_new(1);

		// Set node operation type.
		if (strncmp(par_mgr->curr_token->value, "+", 1) == 0) {
			binop->type = E_ADD_NODE;
		}
		else if (strncmp(par_mgr->curr_token->value, "-", 1) == 0) {
			binop->type = E_MINUS_NODE;
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
			continue;
		}
		
		if (res->type == E_INTERGER_NODE && !par_mgr->lnode_state) {
				par_mgr->curr_expr->data->BinExpNode.left = res;
				par_mgr->lnode_state = 1;
		} 
		else {
			par_mgr->curr_expr->data->BinExpNode.right = res;
			par_mgr->lnode_state = 0;
			par_mgr->expr_depth++;
		}
			
		par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
		res = parse_factor(par_mgr);
		
		if (res->type == E_INTERGER_NODE && !par_mgr->lnode_state) {
			par_mgr->curr_expr->data->BinExpNode.left = res;
			par_mgr->lnode_state = 1;
		} 
		else {
			par_mgr->curr_expr->data->BinExpNode.right = res;
			par_mgr->lnode_state = 0;
			par_mgr->expr_depth++;
		}
					
		// Is it a tree more than 1 nesting of arithmetics.
		if (par_mgr->expr_depth > 1 ) {
			par_mgr->expr_itr->data->BinExpNode.right = par_mgr->curr_expr;
		}		
		else {
			par_mgr->root_expr = par_mgr->curr_expr;
		}

		par_mgr->expr_itr = par_mgr->curr_expr;
			
	}

	// Clean up unused pointers.
	par_mgr->expr_itr = NULL;
	par_mgr->curr_expr = NULL;
	res = NULL;
	
	return par_mgr->root_expr;
}

struct Node *parse_assignment(ParserMgr *par_mgr) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(par_mgr->tok_mgr);
	// Store pointer to subsequent tokens.
	par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
	
	// Final ast build by entire assignment.
	struct Node *ast = NULL;
	// Returned by each recursive handle.
	struct Node *expr = NULL;	
	// Leftmost node of root ast. 
	struct Node *lhand = NULL;

	// Assert we can consume an EQUAL.
	if (parser_can_consume(par_mgr->curr_token->value, "=")) {
		par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
		if ((expr = parse_expr(par_mgr)) != NULL || (expr = parse_string(par_mgr)) != NULL) {
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
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
		}
	}
	else {
		ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
		par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
	}
	return ast;
}

struct Node *parse_group(ParserMgr *par_mgr) {
	// If string isn't next then store error and move to next token.
	if (strcmp(TokenMgr_peek_token(par_mgr->tok_mgr)->type, "STRING") != 0) {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_EMPTY_GROUP);
		TokenMgr_next_token(par_mgr->tok_mgr);
		return NULL;
	}

	// group node itself. i.e {some_group}.
	struct Node *group = Node_new(1);
	// Previously read command <string>.
	struct Node *prev = NULL;
	// Recently read command <string>
	struct Node *curr = NULL;
	
	// Setup group node data.
	group->value = TokenMgr_current_token(par_mgr->tok_mgr)->value;
	group->type = E_GROUP_NODE;

	// Store next token in parser state.
	par_mgr->curr_token =  TokenMgr_next_token(par_mgr->tok_mgr);

	// Iterate through commands and append to group.
	// Below will build a circular linked list.
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && parser_can_consume(par_mgr->curr_token->type, "STRING")) {
		curr = parse_string(par_mgr);
		curr->data = malloc(sizeof(union SyntaxNode));
		if (prev == NULL)
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

int parser_init(TokenMgr *tok_mgr) {

	// Create wrapper structs.
	ParserMgr *par_mgr = ParserMgr_new();
	par_mgr->err_handle = Error_new();
	NodeMgr *node_mgr = NodeMgr_new();

	// This will do for now.
	// TODO: Add descriptive error here.
	if (par_mgr->err_handle == NULL || par_mgr == NULL || node_mgr == NULL || tok_mgr == NULL)
		return -1;
		
	par_mgr->tok_mgr = tok_mgr;
	tok_mgr = NULL;

	
	par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
	
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr)) {
		if (strcmp(par_mgr->curr_token->type, "IDENTIFIER") == 0) {
			par_mgr->curr_expr = parse_assignment(par_mgr);
		}
		else if (strcmp(par_mgr->curr_token->type, "GROUP") == 0) {
			par_mgr->curr_expr = parse_group(par_mgr);
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
		}
		
		if (par_mgr->curr_expr != NULL)
			NodeMgr_add_node(node_mgr, par_mgr->curr_expr);
	}

	int err = par_mgr->err_handle->error_ctr;

	// Free resources.
	Error_free(par_mgr->err_handle);
	NodeMgr_free(node_mgr);
	ParserMgr_free(par_mgr);

	if (err > 0)
		return 1;

	return 0;
}