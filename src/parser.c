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

// NOTE: Below code is just to test the concept of storing vars in a stack.
// A lot of it will need to be refactored and modularised.

struct VDec {
	char name[15];
	char value[50];
	char type[50];
};

struct VGroupDec {
	char name[30];
	char commands[10][100]; //TODO: assuming only 10 commands per group.
	int command_ct;
};

// Below is not ideal because we are using 2 stacks to track variables and groups.
// Groups should be treated similar to variables since type information should not produce different data sets.
// Also we are making assumptions about stack sizes which is also not good.
struct VDec *vstack[1000];

struct VGroupDec *vgstack[100];

static int  stackct = 0;

static int grpstackct = 0;

struct VDec *var_in_stack(char *name) {
	for (int i = 0; i < stackct; i++) {
		if ((strcmp(name, vstack[i]->name) == 0)) {
			return vstack[i];
		}
	}
	return NULL;
}

// --------------------------------------------------------------------------
//   END EXPERIMENTAL CODE
// --------------------------------------------------------------------------

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
	
	// Convert the line number to string.
	int_to_string(off_lineno, offender->lineno);

	// Replace template vars with actual content.
	//TODO: Templating system isn't very flexible in terms of having variable amount of template placeholders.
	template_fmt = string_map_vars(template, template_values, strlen(template), 2);
	
	// Add the final template string to error handler.
	err_handle->errors[err_handle->error_ctr] = malloc(strlen(template_fmt) * sizeof(char));
	strcpy(err_handle->errors[err_handle->error_ctr], template_fmt);
	
	// Increment internal error counter.
	err_handle->error_ctr++;
}

int parser_can_consume(char *tok_type, char *type) {
	if (strcmp(tok_type, type) != 0)
		return 0;
	
	return 1;
}

void parse_group(TokenMgr *tok_mgr, PErrors *err_handle) {
	struct VGroupDec *match = NULL;
	Token *tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	// Manual iteration instead of helper function.
	for (int i = 0; i < grpstackct; i++) {
		if ((strcmp(tok_curr_ptr->value, vgstack[i]->name) == 0)) {
			match = vgstack[i];
		}
	}

	if (match != NULL) {
		parser_add_error(err_handle, tok_curr_ptr, ERR_GROUP_EXIST);
		TokenMgr_next_token(tok_mgr);
		return;
	}

	match = malloc(sizeof(struct VGroupDec));
	match->command_ct = 0;
	strcpy(match->name, tok_curr_ptr->value);
	tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	while (tok_curr_ptr != NULL && strcmp(tok_curr_ptr->type, "STRING") == 0) {
		strcpy(&match->commands[match->command_ct][0], tok_curr_ptr->value);
		match->command_ct++;
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	}

	vgstack[grpstackct++] = match;
}

void parse_assignment(TokenMgr *tok_mgr, PErrors *err_handle) {
	// Store pointer to actual vairable name.
	Token *tok_start_ptr = TokenMgr_current_token(tok_mgr);

	// Store pointer to subsequent tokens.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	// Assert we can "eat" a EQUAL.
	if (parser_can_consume(tok_curr_ptr->value, "=")) {
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
		if (parser_can_consume(tok_curr_ptr->type, "STRING") || parser_can_consume(tok_curr_ptr->type, "INTEGER")) {
			struct VDec *match = var_in_stack(tok_start_ptr->value);

			// Variable already in stack just update value.
			// Otherwise create another VDec instance.
			if (match != NULL) {
				strcpy(match->value, tok_curr_ptr->value);
				strcpy(match->type, tok_curr_ptr->type);
			}
			else {
				struct VDec *var = malloc(sizeof(struct VDec));
				strcpy(var->name, tok_start_ptr->value);
				strcpy(var->value, tok_curr_ptr->value);
				strcpy(var->type, tok_curr_ptr->type);
				vstack[stackct++] = var;
			}
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
	err_handle->errors = malloc(err_handle->error_cap * sizeof(char *));

	// Shorthand pointer to current token.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	while (!TokenMgr_is_last_token(tok_mgr)) {
		if (strcmp(tok_curr_ptr->type, "IDENTIFIER") == 0) {
			parse_assignment(tok_mgr, err_handle);
		}
		else if (strcmp(tok_curr_ptr->type, "GROUP") == 0) {
			parse_group(tok_mgr, err_handle);
		}
		else {
			parser_add_error(err_handle, tok_curr_ptr, ERR_UNEXPECTED);
			TokenMgr_next_token(tok_mgr);
		}

		// Set current pointer to tok_mgr current token.
		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	}

	if (err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {
			printf("%s\n",err_handle->errors[in]);
			free(err_handle->errors[in]);
		}		
		free(err_handle);
		return 1;
	}

	printf("-------------------------------------\n");
	printf("	Variable Stack\n");
	printf("-------------------------------------\n");

	for (int x = 0; x < stackct; x++) {
		printf("Name: %s | Value: %s | Type: %s \n", vstack[x]->name, vstack[x]->value, vstack[x]->type);
		free(vstack[x]);
	}

	printf("-------------------------------------\n");
	printf("	Group Stack\n");
	printf("-------------------------------------\n");
	
	for (int x = 0; x < grpstackct; x++) {
		printf("{%s}\n", vgstack[x]->name);
		for (int c = 0; c < vgstack[x]->command_ct; c++) {
			printf("--> %s\n", vgstack[x]->commands[c]);
		}
		printf("\n");
		free(vgstack[x]);
	}
	return 0;
}