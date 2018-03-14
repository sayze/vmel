#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "vmel.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(1);
	}

	char *b = file_to_buffer(argv[1]);
	TokenMgr *tok_mgr = TokenMgr_new(); 
	Token *tok;
	Token *last;

	build_tokens(b, tok_mgr);
	last = TokenMgr_last_token(tok_mgr);
	// Testing token iteration
	do {
		tok = TokenMgr_next_token(tok_mgr);
		printf("Printing with next token: %s : %s\n", tok->type, tok->value);
	} while(tok != last);

	tok = TokenMgr_first_token(tok_mgr);
	printf("Back to start \n");
	do {
		tok = TokenMgr_next_token(tok_mgr);
		printf("Printing with next token: %s : %s\n", tok->type, tok->value);
	} while(tok != last);
	TokenMgr_free(tok_mgr);
	free(b);
}

void print_usage(void) {
	printf("Usage: vmel [config_file <run.vml>]\n");
}
