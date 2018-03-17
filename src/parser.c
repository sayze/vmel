#include <stdio.h>
#include "tokenizer.h"
#include "utils.h"	
#include "parser.h"
#include "tokens.h"
#include "tokenizer.h"   

int can_consume(char *tok_type, char *type) {
	return strcmp(tok_type, type);
}

int parse_factor(TokenMgr *tok_mgr) {
	Token *temp_tok = *tok_mgr->curr_tok;
	 if (can_consume(temp_tok->type, "INTEGER") == 0) {
		 TokenMgr_next_token(tok_mgr);
		 return string_to_int(temp_tok->value, temp_tok->val_length);
	 }
	 else {
		 return -1;
	 }
}

int parse_term(TokenMgr *tok_mgr) {
	int res = parse_factor(tok_mgr);
	Token *curr = *tok_mgr->curr_tok;
	
	while (strncmp(curr->value, "ASTERISK", curr->val_length) == 0 || 
		strncmp(curr->value, "FSLASH", curr->val_length) == 0) {
	
		// Determine which operation to execute.
		if (strncmp(curr->value, "ASTERISK", curr->val_length) == 0 && 
			can_consume(curr->value, "ASTERISK") == 0) {
			curr = TokenMgr_next_token(tok_mgr);
			res = res * parse_factor(tok_mgr);
		}
		else if (strncmp(curr->value, "FSLASH", curr->val_length) == 0 && 
			can_consume(curr->value, "FSLASH") == 0) {
			curr = TokenMgr_next_token(tok_mgr);
			res = res / parse_factor(tok_mgr);
		}
		curr = *tok_mgr->curr_tok;
	}
	return res;
}


int parse_expr(TokenMgr *tok_mgr) {
	TokenMgr_next_token(tok_mgr);
	int res = parse_term(tok_mgr);
	Token *curr = *tok_mgr->curr_tok;
	while (strncmp(curr->value, "MINUS", curr->val_length) == 0 || 
		strncmp(curr->value, "PLUS", curr->val_length) == 0) {
	
		// Determine which operation to execute.
		if (strncmp(curr->value, "MINUS", curr->val_length) == 0 && 
			can_consume(curr->value, "MINUS") == 0) {
			curr = TokenMgr_next_token(tok_mgr);
			res = res - parse_term(tok_mgr);
		}
		else if (strncmp(curr->value, "PLUS", curr->val_length) == 0 && 
			can_consume(curr->value, "PLUS") == 0) {
			curr = TokenMgr_next_token(tok_mgr);
			res = res + parse_term(tok_mgr);
		}
		curr = *tok_mgr->curr_tok;
	}
  

	return res;
}
