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

// These are the errors a parser may generate. They are mapped to the #DEFINE above.
static const char *Error_Templates[] = {
	"Syntax error : unexpected @0 found in line @1",
	"Error : Duplicate definition {@0} already defined in line @1",
	"Error : Empty group {@0} must contain commands in line @1",
	"Syntax error: Statement '@0' missing arguments in line @1"
};

// Sync ParserMgr internal token to be current token held by TokenMgr.
static void par_mgr_sync(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_current_token(par_mgr->tok_mgr);
}

// Increment the token within TokenMgr and assign it to par_mgr.
static void par_mgr_next(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
}

ParserMgr *ParserMgr_new() {
	ParserMgr *ps = malloc(sizeof(ParserMgr));
	ps->curr_expr = NULL;
	ps->curr_token = NULL;
	ps->tok_mgr = NULL;
	ps->err_handle = NULL;
	return ps;
}

int ParserMgr_free(ParserMgr *par_mgr) {
	if (!par_mgr)
		return 1;
		
	par_mgr->curr_expr = NULL;
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
	if (string_compare(par_mgr->curr_token->type, "STRING")) {
		str = Node_new(0);
		str->type = E_STRING_NODE;
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
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) &&
	(string_compare(par_mgr->curr_token->value, "*") || string_compare(par_mgr->curr_token->value, "/"))) {
		// BinOP node.
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
		par_mgr->expr_depth++;
		res = bop;
	}
	
	return res;
}

Node *parse_expr(ParserMgr *par_mgr) {
	Node *res = parse_term(par_mgr);
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) &&
	(string_compare(par_mgr->curr_token->value, "+") || string_compare(par_mgr->curr_token->value, "-"))) {
		// BinOP node.
		Node *bop = Node_new(1);

		if (strncmp(par_mgr->curr_token->value, "+", 1) == 0) {
			bop->type = E_ADD_NODE;
			bop->value = "+"; 
		}
		else if (strncmp(par_mgr->curr_token->value, "-", 1) == 0){
			bop->type = E_MINUS_NODE;
			bop->value = "-";
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
			continue;
		}
		TokenMgr_next_token(par_mgr->tok_mgr);
		bop->data->BinExpNode.left = res;
		bop->data->BinExpNode.right = parse_term(par_mgr);
		par_mgr->expr_depth++;
		res = bop;
	}
	
	return res;
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
	if (string_compare(par_mgr->curr_token->value, "=")) {
		par_mgr_next(par_mgr);
		if ((expr = parse_string(par_mgr)) != NULL || (expr = parse_expr(par_mgr)) != NULL) {
			
			// Add symbol if not exits.
			// TODO: Move to SyTable module.
			if (!SyTable_get_symbol(par_mgr->sy_table, tok_start_ptr->value)) {
				Symbol *sy = Symbol_new();
				sy->sy_token = tok_start_ptr;
				sy->sy_type = E_IDN_TYPE;
				SyTable_add_symbol(par_mgr->sy_table, sy);
			}
			
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
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			par_mgr_next(par_mgr);
		}
	}
	else {
		ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
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

	// group node itself. i.e {some_group}.
	Node *group = Node_new(1);
	// Previously read command <string>.
	Node *prev = NULL;
	// Recently read command <string>
	Node *curr = NULL;
	
	// Create group entry.
	Symbol *sy = Symbol_new();
	sy->sy_token = par_mgr->curr_token;
	sy->sy_type = E_GROUP_TYPE;
	SyTable_add_symbol(par_mgr->sy_table, sy);
	
	par_mgr_sync(par_mgr);
	
	// Setup group node data.
	group->value = par_mgr->curr_token->value;
	group->type = E_GROUP_NODE;

	// Store next token in parser state.
	par_mgr->curr_token =  TokenMgr_next_token(par_mgr->tok_mgr);

	// Iterate through commands and append to group.
	// Below will build a circular linked list.
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
	// If string isn't next then store error and move to next token.
	if (strcmp(TokenMgr_peek_token(par_mgr->tok_mgr)->type, "STRING") != 0) {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_EMPTY_STMT);
		TokenMgr_next_token(par_mgr->tok_mgr);
		return NULL;
	}

	Node *stmt = Node_new(1);
	stmt->type = E_CMPSTMT_NODE;
	stmt->value = par_mgr->curr_token->value;
	par_mgr_next(par_mgr);
	stmt->data->CmpStmtNode.args = parse_string(par_mgr);
	return stmt;
}

NodeMgr *parser_init(TokenMgr *tok_mgr, SyTable *sy_table) {

	// Create wrapper structs.
	ParserMgr *par_mgr = ParserMgr_new();
	Error *err_handle = Error_new();
	NodeMgr *node_mgr = NodeMgr_new();

	// This will do for now.
	// TODO: Add descriptive error here.
	if (!err_handle || !par_mgr || !node_mgr || !tok_mgr || !sy_table)
		return NULL;
	
	// ParserMgr will track foreign wrapper structs.
	par_mgr->tok_mgr = tok_mgr;
	par_mgr->sy_table = sy_table;
	par_mgr->err_handle = err_handle;
	err_handle = NULL;
	sy_table = NULL;
	tok_mgr = NULL;

	par_mgr_next(par_mgr);	
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr)) {
		if (strcmp(par_mgr->curr_token->type, "IDENTIFIER") == 0) {
			par_mgr->curr_expr = parse_assignment(par_mgr);
		}
		else if (strcmp(par_mgr->curr_token->type, "GROUP") == 0) {
			par_mgr->curr_expr = parse_group(par_mgr);
		}
		else if (strcmp(par_mgr->curr_token->type, "KEYWORD") == 0) {
			par_mgr->curr_expr = parse_keyword(par_mgr);
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			par_mgr_next(par_mgr);
			continue;
		}
		
		if (par_mgr->curr_expr != NULL) {
			par_mgr->curr_expr->depth = par_mgr->expr_depth;
			NodeMgr_add_node(node_mgr, par_mgr->curr_expr);
		}

		par_mgr_sync(par_mgr);
	}
	
	// Any errors then print them.
	if (par_mgr->err_handle->error_ctr > 0)
		Error_print_all(par_mgr->err_handle);

	Error_free(par_mgr->err_handle);
	ParserMgr_free(par_mgr);
	return  node_mgr;
}
