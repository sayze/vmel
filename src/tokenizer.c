#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "tokenizer.h"
#include "tokens.h"
#include "vstring.h"

// Ensure a variable confirms to naming specifications.
static int is_legal_variable(char *var) {
	if (!var) return 0;
	return var[0] != MINUS && !isdigit(var[0]);
}

int TokenMgr_build_tokens(char *buff, TokenMgr *tokmgr) {
	if (null_check(buff, "Tokenizer build tokens") || null_check(tokmgr, "Tokenizer build tokens"))
		return -1;

	// Each character in buffer.
	char c;
	// Reusable storage to hold combined chars. 
	VString store = VString_new();
	// Buff iterator.
	size_t bidx = 0;
	// Error code.
	int error = 0;
	// Track line no.
	int lineno = 1;
	int brlock = 0;

	while (buff[bidx] != '\0' && !error) {
		c = buff[bidx];
		
		VString_set(&store, "");

		// Spaces and irrelevant characters.
		if (c == COMMENT) {
			while (c != NEWLINE && c != '\0') {
				c = buff[++bidx];
			}
			continue;
		}
		else if (isspace(c) && c != NEWLINE) {
			while (isspace(c) && c != NEWLINE) {
				c = buff[++bidx];
			}
			continue;
		}
		else if (c == NEWLINE) {
			lineno++;
			bidx++;
			continue;
		}
		// Simple literals.
		else if (c == LBRACKET) {
			TokenMgr_add_token(tokmgr, E_LBRACKET_TOKEN, "[", lineno);
			bidx++;
		}
		else if (c == LBRACE) {
			TokenMgr_add_token(tokmgr, E_LBRACE_TOKEN, "{", lineno);
			brlock = 1;
			bidx++;
		}
		else if (c == RBRACE) {
			brlock = 0;
			TokenMgr_add_token(tokmgr, E_RBRACE_TOKEN, "}", lineno);
			bidx++;
		}
		else if (c == RBRACKET) {
			TokenMgr_add_token(tokmgr, E_RBRACKET_TOKEN, "]", lineno);
			bidx++;
		}
		else if (c == LPAREN) {
			TokenMgr_add_token(tokmgr, E_LPAREN_TOKEN, "(", lineno);
			bidx++;
		}
		else if (c == RPAREN) {
			TokenMgr_add_token(tokmgr, E_RPAREN_TOKEN, ")", lineno);
			bidx++;
		}
		else if (c == PLUS) {
			TokenMgr_add_token(tokmgr, E_PLUS_TOKEN, "+", lineno);
			bidx++;
		} 
		else if (c ==  MINUS) {
			TokenMgr_add_token(tokmgr, E_MINUS_TOKEN, "-", lineno);
			bidx++;
		} 
		else if (c ==  ASTERISK) {
			TokenMgr_add_token(tokmgr, E_ASTERISK_TOKEN, "*", lineno);
			bidx++;
		} 
		else if (c == FSLASH) {
			TokenMgr_add_token(tokmgr, E_FSLASH_TOKEN, "/", lineno);
			bidx++;
		}
		else if (c == COMMA) {
			TokenMgr_add_token(tokmgr, E_COMMA_TOKEN, ",", lineno);
			bidx++;
		}
		// Compound literals.
		else if (c == BTICK) {
			c = buff[++bidx];
			while (c != BTICK && c != '\0' && c != NEWLINE) {
				VString_pushc(&store, c);
				c = buff[++bidx];
			}
			if (c != BTICK) {
				error = 1;
				continue;
			}
			TokenMgr_add_token(tokmgr, E_MIXSTR_TOKEN, store.str, lineno);
			bidx++;
		}
		else if (c == BANG) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, E_NEQUAL_TOKEN, "!=", lineno);
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, E_NOT_TOKEN, "!", lineno);
			}
			bidx++;
		}
		else if (c == LESSTHAN) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, E_LESSTHANEQ_TOKEN, "<=", lineno);
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, E_LESSTHAN_TOKEN, "<", lineno);
			}
			bidx++;
		}
		else if (c == GREATERTHAN) {
			switch(buff[++bidx]) {
				case EQUAL:
					TokenMgr_add_token(tokmgr, E_GREATERTHANEQ_TOKEN, ">=", lineno);
					break;
				case LESSTHAN:
					TokenMgr_add_token(tokmgr, E_BETWEEN_TOKEN, "><", lineno);
					break;
				default:
					bidx--;
					TokenMgr_add_token(tokmgr, E_GREATERTHAN_TOKEN, ">", lineno);
					break;
			}
			bidx++;
		}
		else if (c ==  EQUAL) {
			if (buff[bidx+1] == EQUAL) {
				TokenMgr_add_token(tokmgr, E_EEQUAL_TOKEN, "==", lineno);
				bidx++;
			}
			else {
				TokenMgr_add_token(tokmgr, E_EQUAL_TOKEN, "=", lineno);
			}	
			bidx++;
		}
		
		else if (c == DQUOTE) {
			c = buff[++bidx];
			while (c != DQUOTE && c != '\0' && c != NEWLINE) {
				VString_pushc(&store, c);
				c = buff[++bidx];
			}
			// Ensure last read char is closing quote.
			if (c != DQUOTE) {
				error = 1;
				continue;
			}
			TokenMgr_add_token(tokmgr, E_STRING_TOKEN, store.str, lineno);
			bidx++;
		}
		else if (c == VAR) {
			c = buff[++bidx];

			while (is_valid_identifier(c)) {
				VString_pushc(&store, c);
				c = buff[++bidx];
			}

			// Prevent empty variables e.g $
			if (store.str_size < 1 || !is_legal_variable(store.str)) {
				error = 1;
				bidx++;
				continue;
			}

			TokenMgr_add_token(tokmgr, E_IDENTIFIER_TOKEN, store.str, lineno);
		}
		else if (isdigit(c)) {
			while (isdigit(c)) {
				VString_pushc(&store, c);
				c = buff[++bidx];
			}
			TokenMgr_add_token(tokmgr, E_INTEGER_TOKEN, store.str, lineno);
		}
		else if (isalpha(c)) {
			if (brlock) {
				while (c != LBRACE && c != RBRACE && c != NEWLINE) {
					VString_pushc(&store, c);
					c = buff[++bidx];	
				}
			}
			else {
				while (is_valid_identifier(c)) {
					VString_pushc(&store, c);
					c = buff[++bidx];
				}
			}
			if (brlock)
				TokenMgr_add_token(tokmgr, E_STRING_TOKEN, store.str, lineno);
			else
				TokenMgr_add_token(tokmgr, E_KEYWORD_TOKEN, store.str, lineno);
		}
		else {
			VString_pushc(&store, c);
			c = buff[++bidx];
			while (c != COMMENT && c != LBRACE && c != RBRACE && c != '\0' \
				&& c != DQUOTE && c != VAR & c != NEWLINE && c!= EQUAL) {
				VString_pushc(&store, c);
				c = buff[++bidx];
			}
			error = 1;
		}
	}

	TokenMgr_add_token(tokmgr, E_EOF_TOKEN, "TAIL", 0);

	if (error)
		printf("Token error: unknown '%s' found in line %d\n", store.str, lineno);

	VString_free(&store);
	return error;
}

TokenMgr *TokenMgr_new(void) {
	TokenMgr *tok_mgr = malloc(sizeof(TokenMgr));
	tok_mgr->toks_tail = NULL;
	tok_mgr->toks_head = NULL;
	tok_mgr->tok_ctr = 0;
	tok_mgr->tok_cap = INIT_TOKMGR_TOKS_SIZE;
	tok_mgr->toks_curr = malloc(tok_mgr->tok_cap * sizeof(Token*));	
	return tok_mgr;
}

int TokenMgr_add_token(TokenMgr *tok_mgr, TokenType tok_type, char *tok_val, int tok_lineno) {
	if (null_check(tok_mgr, "Tokenizer add token")) return -1;

	// Get string length of value.
	size_t tok_val_length = strlen(tok_val);
	
	// Create temp token on heap.
	Token *tmp = malloc(sizeof(Token));	
	tmp->value = calloc(tok_val_length+1, sizeof(char));
	tmp->type = tok_type;
	strcpy(tmp->value, tok_val);
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

	// Only set head if not already defined.
	if (!tok_mgr->toks_head)
		tok_mgr->toks_head = tmp;

	return 0;
}

Token *TokenMgr_peek_token(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer peek token")) return NULL;
	
	// If on last token then just return.
	if (*tok_mgr->toks_curr == tok_mgr->toks_tail)
		return *tok_mgr->toks_curr;

	tok_mgr->toks_curr++;
	Token *next = *tok_mgr->toks_curr;
	tok_mgr->toks_curr--;
	return next;
}

void TokenMgr_print_tokens(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer print token")) return;
	printf("--------------------------------------\n");
	printf("** Token Info Dump **\n");
	printf("--------------------------------------\n");
	printf("Total Tokens: %lu \n", tok_mgr->tok_ctr);
	TokenMgr_reset_curr(tok_mgr);
	for (size_t i = 0; i < tok_mgr->tok_ctr; i++) {
		printf("--> %s\n", tok_mgr->toks_curr[i]->value);
	}
}

int TokenMgr_free(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer free")) return -1;
	
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
	if (null_check(tok_mgr, "Tokenizer next token")) return NULL;
	
	// Don't surpass final token.
	if (*tok_mgr->toks_curr == tok_mgr->toks_tail)
		return NULL;		

	tok_mgr->toks_curr++;	
	return *tok_mgr->toks_curr;
}

Token *TokenMgr_prev_token(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer prev token")) return NULL;

	// Ensure don't surpass first token.
	if (*tok_mgr->toks_curr == tok_mgr->toks_head)
		return *tok_mgr->toks_curr;

	tok_mgr->toks_curr--;
	Token *prev = *tok_mgr->toks_curr;
	tok_mgr->toks_curr++;
	return prev;
}

int TokenMgr_is_last_token(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer last token")) return -1;
	return *tok_mgr->toks_curr  == tok_mgr->toks_tail;
}

Token *TokenMgr_current_token(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer current token")) return NULL;
	return *tok_mgr->toks_curr;
}

void TokenMgr_reset_curr(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "Tokenizer reset")) return;
	while (*tok_mgr->toks_curr != tok_mgr->toks_head) {
		tok_mgr->toks_curr--;
	}
}

Token **grow_curr_tokens(TokenMgr *tok_mgr) {
	if (null_check(tok_mgr, "grow tokens")) return NULL;
	tok_mgr->tok_cap *= 2;
	Token **toks_curr_new = realloc(tok_mgr->toks_curr, sizeof(Token *) * tok_mgr->tok_cap);		
	return toks_curr_new;
}
