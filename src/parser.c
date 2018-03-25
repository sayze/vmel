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
	"empty group {@0} must contain commands in line @1"
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

struct Node *parse_group(TokenMgr *tok_mgr, PErrors *err_handle) {
	Token *tok_peek = TokenMgr_peek_token(tok_mgr);
	// If string isn't next then store error and move to next token.
	if (!parser_can_consume(tok_peek->type, "STRING")) {
		parser_add_perror(err_handle, TokenMgr_current_token(tok_mgr), ERR_EMPTY_GROUP);
		TokenMgr_next_token(tok_mgr);
		return NULL;
	}

	struct Node *group_root = Node_new();
	struct Node *prev = NULL;
	struct Node *cmd_node = NULL;
	
	// Setup root node data.
	group_root->value = TokenMgr_current_token(tok_mgr)->value;
	Token *tok_curr_ptr =  TokenMgr_next_token(tok_mgr);

	// Initialise nodes.
	while (tok_curr_ptr != NULL && strcmp(tok_curr_ptr->type, "STRING") == 0) {
		cmd_node = Node_new();
		cmd_node->value = tok_curr_ptr->value;
		if (prev == NULL) {
			cmd_node->left = group_root;
			group_root->right = cmd_node;
		}
		else {
			cmd_node->left = prev;
			prev->right = cmd_node;
		}
		prev = cmd_node;
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	}

	// Add padding. Useful for freeing and access.
	cmd_node = Node_new();
	cmd_node->value = "EOF";
	cmd_node->left = prev;
	prev->right = cmd_node;

	return group_root;
}

struct Node *parse_assignment(TokenMgr *tok_mgr, PErrors *err_handle) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(tok_mgr);
	// Center node.
	struct Node *assign_node = Node_new();
	// Left node.
	struct Node *left_node = NULL;
	// Right Node.
	struct Node *right_node = NULL;

	// Store pointer to subsequent tokens.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	// Assert we can "eat" an EQUAL.
	if (parser_can_consume(tok_curr_ptr->value, "=")) {
		assign_node->value = "=";
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
		if (parser_can_consume(tok_curr_ptr->type, "STRING") || parser_can_consume(tok_curr_ptr->type, "INTEGER")) {
			// Identifier node. $name
			left_node = Node_new();
			left_node->value = tok_start_ptr->value;

			// Value node. "sam"
			right_node = Node_new();
			right_node->value = tok_curr_ptr->value;

			// Middle glue.
			assign_node->left = left_node;
			assign_node->right = right_node;
		}
		else {
			parser_add_perror(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
		}
	}
	else {
		parser_add_perror(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
	}

	// Iterate to next token.
	TokenMgr_next_token(tok_mgr);

	return assign_node;
}


int parser_init(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		return -1;
	}

	PErrors *err_handle = parser_new_perrors();

	if (err_handle == NULL)
		return -1;

	// Instantiate NodeMgr.
	NodeMgr *node_mgr = NodeMgr_new();
	// Create Node ptr.
	struct Node *ret_node = Node_new();
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