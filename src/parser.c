#include <stdio.h>
#include <stdlib.h>
#include "utils.h"	
#include "parser.h"
#include "tokens.h"
#include "tokenizer.h"   

// Below are the errors which map to Error_Templates.
#define ERR_UNEXPECTED 0
#define ERR_GROUP_EXIST 1

// Initial amount of errors stored.
#define INIT_MAX_ERRORS 20

// --------------------------------------------------------------------------
//   BEGIN EXPERIMENTAL CODE
// --------------------------------------------------------------------------

struct Node {
	struct Node *left;
	char *name;
	struct Node *right;
};

struct Node *new_node() {
	struct Node *n = malloc(sizeof(struct Node));
	n->left = NULL;
	n->right = NULL;
	n->name = NULL;
	return n;
}

// These are the errors a parser may generate. They are mapped to the #DEFINE in parser.h.
static const char *Error_Templates[] = {
	"unexpected @0 found in line @1",
	"duplicate definition {@0} already defined in line @1",
};

void parser_add_error(PErrors *err_handle, Token *offender, int err_type) {
	// Determine if we need to make bigger.
	if (err_handle->error_cap - err_handle->error_ctr <= 5) {
		err_handle->error_cap  = err_handle->error_cap + err_handle->error_ctr / 2;
		char **errors_n = realloc(err_handle->errors, err_handle->error_cap * sizeof(char *));
		err_handle->errors = errors_n;
	}
	
	// String representation of offending line number. 
	char off_lineno[20]; // TODO: Safely assume that a source file won't exceed 9999,9999,9999,9999,9999 lines ?
	// Pointer to offending value.
	char *off_value = offender->value;
	// The error template which is needed.
	const char *template = Error_Templates[err_type];
	// Array containing string of substitute values.
	char *template_values[] = {off_value, off_lineno};
	// Final template error.
	char *template_fmt = NULL;
	
	int_to_string(off_lineno, offender->lineno);

	// Replace template vars with actual content.
	//TODO: Templating system isn't very flexible in terms of having variable amount of template placeholders.
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
	if (err_handle == NULL || !parser_can_consume(TokenMgr_next_token(tok_mgr)->type, "STRING"))
		return NULL;
	
	struct Node *group_root = new_node();
	struct Node *prev = NULL;
	struct Node *cmd_node = NULL;
	
	// Setup root node data.
	group_root->name = TokenMgr_current_token(tok_mgr)->value;
	Token *tok_curr_ptr =  TokenMgr_next_token(tok_mgr);

	// Initialise nodes.
	while (tok_curr_ptr != NULL && strcmp(tok_curr_ptr->type, "STRING") == 0) {
		cmd_node = new_node();
		cmd_node->name = tok_curr_ptr->value;
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
	cmd_node = new_node();
	cmd_node->name = "EOF";
	cmd_node->left = prev;
	prev->right = cmd_node;

	return group_root;
}

struct Node *parse_assignment(TokenMgr *tok_mgr, PErrors *err_handle) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(tok_mgr);
	// Center node.
	struct Node *assign_node = new_node();
	// Left node.
	struct Node *left_node = NULL;
	// Right Node.
	struct Node *right_node = NULL;

	// Store pointer to subsequent tokens.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	// Assert we can "eat" an EQUAL.
	if (parser_can_consume(tok_curr_ptr->value, "=")) {
		assign_node->name = "=";
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
		if (parser_can_consume(tok_curr_ptr->type, "STRING") || parser_can_consume(tok_curr_ptr->type, "INTEGER")) {
			// Identifier node. $name
			left_node = new_node();
			left_node->name = tok_start_ptr->value;

			// Value node. "sam"
			right_node = new_node();
			right_node->name = tok_curr_ptr->value;

			// Middle glue.
			assign_node->left = left_node;
			assign_node->right = right_node;
		}
		else {
			parser_add_error(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
		}
	}
	else {
		parser_add_error(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
	}

	// Iterate to next token.
	TokenMgr_next_token(tok_mgr);

	return assign_node;
}


int parser_init(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		return 1;
	}

	// Initialise Error handler.
	// TODO: Create new function.
	PErrors *err_handle = malloc(sizeof(PErrors));
	err_handle->error_cap = INIT_MAX_ERRORS;
	err_handle->error_ctr = 0;
	err_handle->errors = malloc(sizeof(char *) * err_handle->error_cap);

	struct Node *ret_node = NULL;
	struct Node *root[1000];
	int rct = 0;

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
			parser_add_error(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
			TokenMgr_next_token(tok_mgr);
		}

		if (ret_node != NULL)
			root[rct++] = ret_node;
		// Set current pointer to tok_mgr current token.
		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
		

	}

	// We are "cheating" by assuming types using name.
	// but this is only poc.
	for (int n =0; n < rct; n++) {
		struct Node *rn = root[n];
		if (strcmp(rn->name, "=") == 0) {
			free(rn->left);
			free(rn->right);
		}
		else {
			struct Node *itr = rn->right;
			while (strncmp(itr->name, "EOF", 3) != 0) {
				itr = itr->right;
				free(itr->left);
			}
			free(itr);
		}

		// Free root node.
		free(rn);
	}

	if (err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {
			printf("%s\n",err_handle->errors[in]);
			free(err_handle->errors[in]);
		}
		free(err_handle);		
		return 1;
	}

	return 0;
}