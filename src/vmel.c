#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "utils.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(1);
	}

	char *b = file_to_buffer(argv[1]);
	TokenMgr *tok_mgr = TokenMgr_new(); 
	Token *tok;

	build_tokens(b, tok_mgr);
	
	// Testing token iteration
	while ((tok = TokenMgr_next_token(tok_mgr)) != NULL) {
		printf("Printing with next token: %s : %s\n", tok->type, tok->value);
	}
	TokenMgr_reset_token(tok_mgr);
	printf("Back to start \n");
	while ((tok = TokenMgr_next_token(tok_mgr)) != NULL) {
		printf("Printing with next token: %s : %s\n", tok->type, tok->value);
	}
	TokenMgr_free(tok_mgr);
	free(b);
}
