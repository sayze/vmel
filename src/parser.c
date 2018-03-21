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
	tok_mgr_ptr = TokenMgr_current_token(tok_mgr);
	int result = 0;

	 if (can_consume(tok_mgr_ptr->type, "INTEGER") == 0) {
		 TokenMgr_next_token(tok_mgr);
		 return string_to_int(tok_mgr_ptr->value, tok_mgr_ptr->val_length);
	 }
	 else if (can_consume(tok_mgr_ptr->type, "LPAREN") == 0) {
		 TokenMgr_next_token(tok_mgr);
		 result = parse_expr(tok_mgr);
		 tok_mgr_ptr = TokenMgr_current_token(tok_mgr);
		 if (can_consume(tok_mgr_ptr->type, "RPAREN") == 0) {
			TokenMgr_next_token(tok_mgr); 
		 }else{
			 return -1;
		 }
		// Return final result.
		return result;
	 }
	 else {
		 return -1;
	 }
}

int parse_term(TokenMgr *tok_mgr) {
	int res = parse_factor(tok_mgr);
	tok_mgr_ptr = TokenMgr_current_token(tok_mgr);
	
	// Handle expression calculation.
	while (strncmp(tok_mgr_ptr->value, "ASTERISK", tok_mgr_ptr->val_length) == 0 || 
		strncmp(tok_mgr_ptr->value, "FSLASH", tok_mgr_ptr->val_length) == 0) {
		if (strncmp(tok_mgr_ptr->value, "ASTERISK", tok_mgr_ptr->val_length) == 0 && 
			can_consume(tok_mgr_ptr->value, "ASTERISK") == 0) {
			tok_mgr_ptr = TokenMgr_next_token(tok_mgr);
			res = res * parse_factor(tok_mgr);
		}
		else if (strncmp(tok_mgr_ptr->value, "FSLASH", tok_mgr_ptr->val_length) == 0 && 
			can_consume(tok_mgr_ptr->value, "FSLASH") == 0) {
			tok_mgr_ptr = TokenMgr_next_token(tok_mgr);
			res = res / parse_factor(tok_mgr);
		}
		tok_mgr_ptr = TokenMgr_current_token(tok_mgr);
	}
	return res;
}


int parse_expr(TokenMgr *tok_mgr) {
	
	// Only retrieve next token on initial entry.
	if (tok_mgr->curr_tok == NULL)
		TokenMgr_next_token(tok_mgr);

	int res = parse_term(tok_mgr);
	tok_mgr_ptr = TokenMgr_current_token(tok_mgr);
	
	// Handle expression calculation.
	while (strncmp(tok_mgr_ptr->value, "MINUS", tok_mgr_ptr->val_length) == 0 || 
		strncmp(tok_mgr_ptr->value, "PLUS", tok_mgr_ptr->val_length) == 0) {
		if (strncmp(tok_mgr_ptr->value, "MINUS", tok_mgr_ptr->val_length) == 0 && 
			can_consume(tok_mgr_ptr->value, "MINUS") == 0) {
			tok_mgr_ptr = TokenMgr_next_token(tok_mgr);
			res = res - parse_term(tok_mgr);
		}
		else if (strncmp(tok_mgr_ptr->value, "PLUS", tok_mgr_ptr->val_length) == 0 && 
			can_consume(tok_mgr_ptr->value, "PLUS") == 0) {
			tok_mgr_ptr = TokenMgr_next_token(tok_mgr);
			res = res + parse_term(tok_mgr);
		}
		tok_mgr_ptr = TokenMgr_current_token(tok_mgr);
	}
  

	return res;
}
