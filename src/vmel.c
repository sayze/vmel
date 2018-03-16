#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "utils.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(2);
	}

	// Read source file to buffer.
	char *b = file_to_buffer(argv[1]);
	// Instantiate new token manager.
	TokenMgr *tok_mgr = TokenMgr_new(); 
	// Attempt to fill token manager with tokens.
	build_tokens(b, tok_mgr);

	// Parse tokens.
	TokenMgr_print_tokens(tok_mgr);

	// Free resources.
	TokenMgr_free(tok_mgr);
	free(b);
}
