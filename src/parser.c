#include <stdio.h>
#include <stdlib.h>
#include "utils.h"	
#include "parser.h"
#include "tokens.h"
#include "tokenizer.h"   


/**
 * NOTE: Below code is just to test the concept of storing vars in a stack.
 * A lot of it will need to be refactored and modularised.
 */
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

char errors[20];

// Below is not ideal because we are using 2 stacks to track variables and groups.
// Groups should be treated similar to variables since type information should not produce different data sets.
// Also we are making assumptions about stack sizes which is also not good.
struct VDec *vstack[100];

struct VGroupDec *vgstack[100];

static int  stackct = 0;

static int grpstackct = 0;

/* END of bad code */

int can_consume(char *tok_type, char *type) {
	if (strcmp(tok_type, type) != 0)
		return 0;
	
	return 1;
}

struct VDec *var_in_stack(char *name) {
	for (int i = 0; i < stackct; i++) {
		if ((strcmp(name, vstack[i]->name) == 0)) {
			return vstack[i];
		}
	}
	return NULL;
}

void parse_group(TokenMgr *tok_mgr) {
	struct VGroupDec *match = NULL;
	Token *tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	// Manual iteration instead of helper function.
	for (int i = 0; i < grpstackct; i++) {
		if ((strcmp(tok_curr_ptr->value, vgstack[i]->name) == 0)) {
			match = vgstack[i];
		}
	}

	//TODO: Should probably have a dedicated error buffer to store all messages.
	// This way they all get aggregated and printed in the end.
	if (match != NULL) {
		printf("** Error: Duplicate group %s already defined in line %d\n", tok_curr_ptr->value, tok_curr_ptr->lineno);
		error_stat = 1;
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

void parse_assignment(TokenMgr *tok_mgr) {
	// Store pointer to actual vairable name.
	Token *tok_start_ptr = TokenMgr_current_token(tok_mgr);

	// Store pointer to subsequent tokens.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	// Assert we can "eat" a EQUAL.
	if (can_consume(tok_curr_ptr->value, "EQUAL")) {
		tok_curr_ptr = TokenMgr_next_token(tok_mgr);
		if (can_consume(tok_curr_ptr->type, "STRING") || can_consume(tok_curr_ptr->type, "INTEGER")) {
			struct VDec *match = var_in_stack(tok_curr_ptr->value);

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
			error_stat = 1;
		}
	}
	else {
		error_stat = 1;
	}

	// Iterate to next token.
	TokenMgr_next_token(tok_mgr);
}


int parser_init(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		return 1;
	}

	// Shorthand pointer to current token.
	Token *tok_curr_ptr = TokenMgr_next_token(tok_mgr);
	
	while (!TokenMgr_is_last_token(tok_mgr)) {
		if (strcmp(tok_curr_ptr->type, "IDENTIFIER") == 0) {
			parse_assignment(tok_mgr);
		}
		else if (strcmp(tok_curr_ptr->type, "GROUP") == 0) {
			parse_group(tok_mgr);
			if (error_stat != 0)
				tok_curr_ptr = NULL;
		}
		else {
			printf ("**Error: unexpected \"%s\" found in line %d\n", tok_curr_ptr->value, tok_curr_ptr->lineno);
			TokenMgr_next_token(tok_mgr);
		}

		// Set current pointer to tok_mgr current token.
		tok_curr_ptr = TokenMgr_current_token(tok_mgr);
	}

	if (error_stat != 0)
		return 1;

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