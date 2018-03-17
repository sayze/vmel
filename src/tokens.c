#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"
#include "utils.h"

// Comment.
#define COMMENT '#'
// Newline.
#define NEWLINE '\n'
// Double quotes.
#define DQUOTE '"'
// Equals operator.
#define EQUAL '='
// Left brace.
#define LBRACE '{'
// Right brace.
#define RBRACE '}'
// Var identifier.
#define VAR '$'

int build_tokens(char *buff, TokenMgr *tokmgr) {
	if (buff == NULL) {
		printf("** Error Buffer invalid state cannot build tokens\n");
		return 1;
	}

	if (tokmgr == NULL) {
		printf("** Error Invalid token manager passed to build token");
		return 1;
	}

	// Each character in buffer.
	char c;
	// Reusable storage to hold strings. 
	char store[100];
	// Buff iterator.
	int bidx = 0;
	// Track storage size.
	int stctr = 0;
	// Error code.
	int error = 0;
	// Track line no.
	int lineno = 1;
	
	// Iterate through all chars until terminator or error.
	while (buff[bidx] != '\0' && !error) {
		c = buff[bidx];

		if (c == COMMENT) {
			while (c != NEWLINE) {
				c = buff[++bidx];
			}
			continue;

		} 
		else if (c == NEWLINE) {
			lineno++;
			bidx++;
			continue;

		} 
		else if (isspace(c)) {
			bidx++;
			continue;

		} 
		else if (c == EQUAL) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "=");
			bidx++;
		}
		else if (c == DQUOTE) {
			c = buff[++bidx];
			while (c != DQUOTE) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "STRING", store);
			stctr = 0;
			bidx++;
		}
		else if (c == VAR) {
			c = buff[++bidx];
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "IDENTIFIER", store);
			stctr = 0;
		}
		else if (isdigit( (int) c)) {
			while (isdigit( (int) c)) {
				store[stctr++] = c;
				c = (int) buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "INTEGER", store);
			stctr = 0;
		}
		else if (c == LBRACE) {
			c = buff[++bidx];
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "GROUP", store);
			stctr = 0;
			bidx++;
		}
		else {
			error = 1;
			printf("** Invalid syntax: unknown '%c' found in line %d\n", c, lineno);
		}
	}

	return error;
}

TokenMgr *TokenMgr_new() {
	TokenMgr *tok_mgr = malloc(sizeof(TokenMgr));
	tok_mgr->tok_ctr = 0;
	tok_mgr->curr_tok = NULL;
	return tok_mgr;
}

int TokenMgr_add_token(TokenMgr *tok_mgr, char tok_type[50], char tok_val[100]) {
	if (tok_mgr == NULL) {
		printf("**Error** Invalid token manager passed to TokenMgr_add_token");
		return 1;
	}

	Token *tmp = malloc(sizeof(Token));
	strcpy(tmp->type, tok_type);
	strcpy(tmp->value, tok_val);
	tok_mgr->toks[tok_mgr->tok_ctr] = tmp;
	tok_mgr->tok_ctr += 1;
	return 0;
}

void TokenMgr_print_tokens(TokenMgr *tok_mgr) {
	for (size_t i =0; i < tok_mgr->tok_ctr; i++) {
		printf("%s %s \n", tok_mgr->toks[i]->type, tok_mgr->toks[i]->value);
	}
}



int TokenMgr_free(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL) {
		printf("**Error** Invalid token manager passed to TokenMgr_free");
		return 1;
	}

	for (size_t i =0; i < tok_mgr->tok_ctr; i++) {
		free(tok_mgr->toks[i]);
	}
	tok_mgr->curr_tok = NULL;
	free(tok_mgr);
	return 0;
}

Token *TokenMgr_next_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return NULL;

	if (tok_mgr->curr_tok == NULL) {
		tok_mgr->curr_tok = tok_mgr->toks;
	} 
	else {

		// Don't surpass final token.
		if (*tok_mgr->curr_tok == tok_mgr->toks[tok_mgr->tok_ctr-1]) {
			return NULL;
		}
		
		tok_mgr->curr_tok++;	
	}
	return *tok_mgr->curr_tok;
}

Token *TokenMgr_prev_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return NULL;

	if (tok_mgr->curr_tok == NULL) {
		tok_mgr->curr_tok = &tok_mgr->toks[tok_mgr->tok_ctr];
	} 
	else {

		// Don't surpass first token.
		if (*tok_mgr->curr_tok == tok_mgr->toks[0]) {
			return NULL;
		}
			
		tok_mgr->curr_tok--;	
	}
	return *tok_mgr->curr_tok;
}

void TokenMgr_reset_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL)
		return;
	tok_mgr->curr_tok = NULL;
}

Token *get_first_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL || tok_mgr->tok_ctr < 1)
		return NULL;
	
	return tok_mgr->toks[0];
}

Token *get_last_token(TokenMgr *tok_mgr) {
	if (tok_mgr == NULL || tok_mgr->tok_ctr < 1)
		return NULL;

	return tok_mgr->toks[tok_mgr->tok_ctr-1];
}
