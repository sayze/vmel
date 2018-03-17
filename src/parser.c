#include <stdio.h>
#include "tokenizer.h"
#include "utils.h"	
#include "parser.h"
    
int can_consume (Token *tok, char *type) {
    return strcmp(tok->type, type);
}

int parse_expr (TokenMgr *tok_mgr) {
	// Parse Action !
	Token *curr = TokenMgr_next_token(tok_mgr);

	Token *left = curr;
	if (can_consume(left, "INTEGER") == 0)
		curr = TokenMgr_next_token(tok_mgr);
    else
        return -1;

	Token *op = curr;
	if (can_consume(op, "OPERATOR") == 0 )
		curr = TokenMgr_next_token(tok_mgr);
	else
        return -1;

	Token *right = curr;
	if (can_consume(right, "INTEGER") == 0)
		curr = TokenMgr_next_token(tok_mgr);
	else
        return -1;

	int res =  string_to_int(left->value, left->val_length) + string_to_int(right->value, right->val_length);
	return res;
}

// int parse_assignment(TokenMgr  *tok_mgr) {

// }

// void init_parse(TokenMgr *tok_mgr) {

// }