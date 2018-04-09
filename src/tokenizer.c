#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "tokenizer.h"
#include "tokens.h"

// Below are reserved keywords.
// TODO: Move this to tokens.h ? or better manage this.
static const char *R_Keywords[KWORDS_SIZE] = {"print"};

//TODO: make below lexing more efficient and readable.
// possibly create functions for each category and/or
// add token only at once at the end of char read.
int TokenMgr_build_tokens(char *buff, TokenMgr *tokmgr) {
	if (!buff) {
		printf("** Error Buffer invalid state cannot build tokens\n");
		return 1;
	}

	if (!tokmgr) {
		printf("** Error Invalid token manager passed to build token");
		return 1;
	}

	// Each character in buffer.
	char c;
	// Reusable storage to hold strings. 
	char store[100]; // TODO: Make dynamic incase someone has string longer than 100 chars.
	// Buff iterator.
	int bidx = 0;
	// Track storage size.
	int stctr = 0;
	// Error code.
	int error = 0;
	// Track line no.
	int lineno = 1;

	// Add a head token as padding to help with pointer arithmetic.
	if (!tokmgr->toks_head) {
		TokenMgr_add_token(tokmgr, "HEAD", "HEAD", 0);
		tokmgr->toks_head = *tokmgr->toks_curr;
	}

	// Iterate through all chars until terminator or error.
	while (buff[bidx] != '\0' && !error) {
		c = buff[bidx];

		if (c == COMMENT) {
			while (c != NEWLINE && c != '\0') {
				c = buff[++bidx];
			}
			continue;

		} 
		else if (c == NEWLINE) {
			lineno++;
			bidx++;
			continue;
		}
		else if (c == BANG) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, "OPERATOR", "!=", lineno);
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, "OPERATOR", "!", lineno);
			}
			bidx++;
		}
		else if (isspace(c)) {
			while (isspace(c)) {
				c = buff[++bidx];
			}
			continue;

		}
		else if (c == LBRACKET) {
			TokenMgr_add_token(tokmgr, "LBRACKET", "[", lineno);
			bidx++;
		}
		else if (c == RBRACKET) {
			TokenMgr_add_token(tokmgr, "RBRACKET", "]", lineno);
			bidx++;
		}
		else if (c == LPAREN) {
			TokenMgr_add_token(tokmgr, "LPAREN", "(", lineno);
			bidx++;
		}
		else if (c == RPAREN) {
			TokenMgr_add_token(tokmgr, "RPAREN", ")", lineno);
			bidx++;
		}
		else if (c == LESSTHAN) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, "OPERATOR", "<=", lineno);
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, "OPERATOR", "<", lineno);
			}
			bidx++;
		}
		else if (c == GREATERTHAN) {
			switch(buff[++bidx]) {
				case EQUAL:
					TokenMgr_add_token(tokmgr, "OPERATOR", ">=", lineno);
					break;
				case LESSTHAN:
					TokenMgr_add_token(tokmgr, "OPERATOR", "><", lineno);
					break;
				default:
					bidx--;
					TokenMgr_add_token(tokmgr, "OPERATOR", ">", lineno);
					break;
			}
			bidx++;
		}
		else if (c ==  EQUAL) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, "OPERATOR", "==", lineno);
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, "OPERATOR", "=", lineno);
			}	
			bidx++;
		}
		else if (c == PLUS) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "+", lineno);
			bidx++;
		} 
		else if (c ==  MINUS) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "-", lineno);
			bidx++;
		} 
		else if (c ==  ASTERISK) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "*", lineno);
			bidx++;
		} 
		else if (c == FSLASH) {
			TokenMgr_add_token(tokmgr, "OPERATOR", "/", lineno);
			bidx++;
		}
		else if (c == COMMA) {
			TokenMgr_add_token(tokmgr, "COMMA", ",", lineno);
			bidx++;
		}
		else if (c == DQUOTE) {
			c = buff[++bidx];
			while (c != DQUOTE && c != '\0' && c != NEWLINE) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			
			// Ensure last read char is closing quote.
			if (c != DQUOTE) {
				error = 1;
				continue;
			}
			TokenMgr_add_token(tokmgr, "STRING", store, lineno);
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
			TokenMgr_add_token(tokmgr, "IDENTIFIER", store, lineno);
			stctr = 0;
		}
		else if (isdigit(c)) {
			while (isdigit(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			TokenMgr_add_token(tokmgr, "INTEGER", store, lineno);
			stctr = 0;
		}
		else if (c == LBRACE) {
			c = buff[++bidx];
			
			// Catch use cases where { NON-ALPHA }  
			if (!is_valid_identifier(c)) {
				store[stctr++] = c;
				store[stctr] = '\0';
				error = 1;
				continue;
			}

			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = buff[++bidx];
			}

			store[stctr] = '\0';
			// Ensure last read char is right brace.
			if (c != RBRACE) {
				error = 1;
				continue;
			}

			TokenMgr_add_token(tokmgr, "GROUP", store, lineno);
			stctr = 0;
			bidx++;
		}
		else if (isalpha(c)) {
			while (is_valid_identifier(c)) {
				store[stctr++] = c;
				c = (int) buff[++bidx];
			}
			store[stctr] = '\0';
			if (is_valid_keyword(store)) {
				TokenMgr_add_token(tokmgr, "KEYWORD", store, lineno);
			}
			else {
				error = 1;
			}
			stctr = 0;
		}
		else {
			store[stctr++] = c;
			c = buff[++bidx];
			while (c != COMMENT && c != LBRACE && c != RBRACE && c != '\0' \
				&& c != DQUOTE && c != VAR & c != NEWLINE && c!= EQUAL) {
				store[stctr++] = c;
				c = buff[++bidx];
			}
			store[stctr] = '\0';
			error = 1;
		}
	}

	// Add tail token.
	TokenMgr_add_token(tokmgr, "EOT", "EOT", 0);

	if (error)
		printf("Syntax error: unknown '%s' found in line %d\n", store, lineno);

	return error;
}

TokenMgr *TokenMgr_new(void) {
	TokenMgr *tok_mgr = malloc(sizeof(TokenMgr));
	tok_mgr->toks_tail = NULL;
	tok_mgr->toks_head = NULL;
	tok_mgr->tok_ctr = 0;
	tok_mgr->tok_cap = TOKMGR_TOKS_INIT_SIZE;
	tok_mgr->toks_curr = malloc(tok_mgr->tok_cap * sizeof(Token*));	
	return tok_mgr;
}

int TokenMgr_add_token(TokenMgr *tok_mgr, char tok_type[20], char *tok_val, int tok_lineno) {
	if (!tok_mgr) {
		printf("**Error** Invalid token manager passed to TokenMgr_add_token");
		return 1;
	}	

	// Get string length of value.
	size_t tok_val_length = strlen(tok_val);
	
	// Create temp token on heap.
	Token *tmp = malloc(sizeof(Token));	
	tmp->value = calloc(tok_val_length+1, sizeof(char));
	strcpy(tmp->type, tok_type);
	strcpy(tmp->value, tok_val);
	tmp->val_length = strlen(tmp->value);
	tmp->lineno = tok_lineno;

	// Determine if we need more room in toks.
	if (tok_mgr->tok_cap - tok_mgr->tok_ctr <= 5) {
		tok_mgr->toks_curr = grow_curr_tokens(tok_mgr);

		// Make sure grow was succesful.
		if (!tok_mgr->toks_curr)
			return 1;
	}	
	
	tok_mgr->toks_curr[tok_mgr->tok_ctr++] = tmp;
	tok_mgr->toks_tail = tmp;

	return 0;
}

Token *TokenMgr_peek_token(TokenMgr *tok_mgr) {
	if (!tok_mgr || *tok_mgr->toks_curr == tok_mgr->toks_tail)
		return NULL;

	tok_mgr->toks_curr++;
	Token *next = *tok_mgr->toks_curr;
	tok_mgr->toks_curr--;
	return next;
}

void TokenMgr_print_tokens(TokenMgr *tok_mgr) {
	TokenMgr_reset_curr(tok_mgr);
	printf("--------------------------------------\n");
	printf("** Token Info Dump **\n");
	printf("--------------------------------------\n");
	for (size_t i = 1; tok_mgr->toks_curr[i] != tok_mgr->toks_tail; i++) {
		printf("--> %s %s \n", tok_mgr->toks_curr[i]->type, tok_mgr->toks_curr[i]->value);
	}
}

int TokenMgr_free(TokenMgr *tok_mgr) {
	if (!tok_mgr) {
		printf("**Error** Invalid token manager passed to TokenMgr_free");
		return 1;
	}
	
	TokenMgr_reset_curr(tok_mgr);
	
	for (size_t i = 0; i < tok_mgr->tok_ctr; i++) {
		free(tok_mgr->toks_curr[i]->value);
		free(tok_mgr->toks_curr[i]);
	}

	// Free resources.
	free(tok_mgr->toks_curr);
	tok_mgr->toks_curr = NULL;
	tok_mgr->toks_head = NULL;
	tok_mgr->toks_tail = NULL;
	free(tok_mgr);
	tok_mgr = NULL;

	return 0;
}

Token *TokenMgr_next_token(TokenMgr *tok_mgr) {
	if (!tok_mgr)
		return NULL;
	
	// Don't surpass final token.
	if (TokenMgr_is_last_token(tok_mgr))
		return NULL;		

	tok_mgr->toks_curr++;	
	return *tok_mgr->toks_curr;
}

Token *TokenMgr_prev_token(TokenMgr *tok_mgr) {
	// Ensure is initialised or don't surpass first token.
	if (!tok_mgr || *tok_mgr->toks_curr == tok_mgr->toks_head)
		return NULL;

	tok_mgr->toks_curr--;
	return *tok_mgr->toks_curr;
}

int TokenMgr_is_last_token(TokenMgr *tok_mgr) {
	if (!tok_mgr)
		return -1;

	return *tok_mgr->toks_curr == tok_mgr->toks_tail;
}

Token *TokenMgr_current_token(TokenMgr *tok_mgr) {
	if (!tok_mgr)
		return NULL;

	return *tok_mgr->toks_curr;
}

void TokenMgr_reset_curr(TokenMgr *tok_mgr) {
	if (!tok_mgr)
		return;
	
	while (*tok_mgr->toks_curr != tok_mgr->toks_head) {
		tok_mgr->toks_curr--;
	}
}

Token **grow_curr_tokens(TokenMgr *tok_mgr) {
	if (!tok_mgr)
		return NULL;
		
	tok_mgr->tok_cap *= 2;
	Token **toks_curr_new = realloc(tok_mgr->toks_curr, sizeof(Token *) * tok_mgr->tok_cap);		
	return toks_curr_new;
}

int is_valid_keyword(char *str) {
	int ret = 0;
	
	if (!str)
		return ret;
	
	for (int x = 0; x < KWORDS_SIZE-1; x++) {
		const char *t = R_Keywords[x];
		if (strcmp(t, str) == 0) {
			ret = 1;
			break;
		}
	}
	return ret;
}

int is_valid_identifier(char id) {
	return (isalpha(id) || id == '_' || id == '-' || isdigit(id));
}
